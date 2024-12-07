#include "BTreePage.h"
#include "BTreeRecord.h"
#include "dataManager.h"
#include "Program.h"

DataManager::DataManager() {
    this->dataFilename = "../data.dat";
    this->bTreeFilename = "../bTreeData.dat";
    this->ensureFileExist(this->bTreeFilename, this->bTreeFileStream);
    this->ensureFileExist(this->dataFilename, this->dataFileStream);
    this->lastDataPageNumber = 1;
    this->dataPageRecordsCount = 0;
    this->nextFreeBTreePageNumber = 1;  // strony dla węzłów adresujemy od 1, by łatwiej uwzględniać brak rodzica
}

DataManager::~DataManager() {
    if (this->bTreeFileStream.is_open()) {
        this->bTreeFileStream.close();
    }
    if (this->dataFileStream.is_open()) {
        this->dataFileStream.close();
    }
}

void DataManager::ensureFileExist(string filename, fstream &fileStream) {
    fileStream.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!fileStream) {
        fileStream.clear();
        fileStream.open(filename, std::ios::binary | std::ios::out);
    }
    fileStream.close();
}

BTreePage* DataManager::loadBTreePage(int pageAddress) {
    if (pageAddress == 0) {
        return nullptr;
    }
    this->bTreeFileStream.open(this->bTreeFilename, std::ios::binary | std::ios::in);
    bTreeFileStream.seekg(BTREE_PAGE_SIZE * (pageAddress - 1)); // strony są numerowane od 1, więc musimy zmniejszyć adres początkowy
    int bTreePageData[BTREE_PAGE_SIZE/sizeof(int)];
    this->bTreeFileStream.read(reinterpret_cast<char*>(bTreePageData), BTREE_PAGE_SIZE);
    BTreePage* pageRead = new BTreePage();
    // odczytanie nagłówka strony b drzewa
    pageRead->setPageId(bTreePageData[0]);
    pageRead->setKeysNumber(bTreePageData[1]);
    pageRead->setParentId(bTreePageData[2]);
    pageRead->setChildrenNumber(bTreePageData[3]);
    // odczytanie danych węzła b drzewa
    int keysNumber = pageRead->getKeysNumber();
    for (int i = 0; i < keysNumber; i++) {          // wpisujemy do wektora tylko tyle kluczy, ile rzeczywiście zawiera węzeł b drzewa
        pageRead->addNewRecord(bTreePageData[4 + 2 * i], bTreePageData[4 + 2 * i + 1]);
    }
    vector<int>* childrensIs = pageRead->getChildrenIds();
    int offset = 4 + BTREE_ORDER * 4;                   // offset do miejsca w którym zaczynamy czytać id węzłów potomnych
    for (int i = 0; i < pageRead->getChildrenNumber(); i++) {  // węzeł będzie mieć zawsze o 1 więcej węzłów potomnych niż kluczy
        childrensIs->push_back(bTreePageData[offset + i]);
    }
    this->bTreeFileStream.close();
    return pageRead;
    // NIE WIEM CZY POPRAWNIE BĘDĄ SIĘ DODAWAĆ ID WĘZŁÓW POTOMNYCH TUTAJ, ALE CHYBA POWINNY
}

void DataManager::saveBTreePage(BTreePage *page) {
    this->bTreeFileStream.open(this->bTreeFilename, std::ios::binary | std::ios::app);
    this->bTreeFileStream.seekp((page->getPageId() - 1) * BTREE_PAGE_SIZE);
    int bTreePageData[BTREE_PAGE_SIZE/sizeof(int)];
    bTreePageData[0] = page->getPageId();
    bTreePageData[1] = page->getKeysNumber();
    bTreePageData[2] = page->getParentId();
    bTreePageData[3] = page->getChildrenNumber();
    vector<BTreeRecord>* records = page->getRecords();
    for (int i = 0; i < records->size(); i++) {
        bTreePageData[4 + 2 * i] = records->at(i).getKey();
        bTreePageData[4 + 2 * i + 1] = records->at(i).getPageNumberInFile();
    }
    // przesunięcie wskaźnika poza nagłówek i sekcję, gdzie zostają zapisane klucze wraz z ich adresami
    int offset = 4 + BTREE_ORDER * 4;
    // 3 to długość nagłówka, mnożymy przez cztery, bo maks liczba kluczy to 2d i każdy rekord zajmuje dwie zmienne typu int
    vector<int>* childrensIds = page->getChildrenIds();
    for (int i = 0; i < childrensIds->size(); i++) {
        bTreePageData[offset + i] = childrensIds->at(i);
    }
    this->bTreeFileStream.write(reinterpret_cast<char*>(bTreePageData), BTREE_PAGE_SIZE);
    this->bTreeFileStream.close();
}

int DataManager::getNextFreePageNumber() {
    return this->nextFreeBTreePageNumber;
}

void DataManager::increasePageNumber() {
    this->nextFreeBTreePageNumber += 1;
}

vector<FileRecord>* DataManager::readRecordsDiskPage(int diskPageNumber) {
    this->dataFileStream.open(this->dataFilename, std::ios::binary | std::ios::in);
    this->dataFileStream.seekg((diskPageNumber - 1) * DATA_PAGE_SIZE);
    recordData recordsData[DATA_PAGE_SIZE/DATA_RECORD_SIZE];
    this->bTreeFileStream.read(reinterpret_cast<char*>(recordsData), DATA_PAGE_SIZE);
    int recordsRead = this->dataFileStream.gcount() / DATA_RECORD_SIZE;
    vector<FileRecord>* records = new vector<FileRecord>(recordsRead);
    for (int i = 0; i < recordsRead; i++) {
        FileRecord record = FileRecord(recordsData[i].a, recordsData[i].b, recordsData[i].h, recordsData[i].key);
        records->push_back(record);
    }
    this->dataFileStream.close();
    return records;
}

void DataManager::saveRecordsDiskPage(vector<FileRecord>* page, int pageNumber) {
    this->dataFileStream.open(this->dataFilename, std::ios::binary | std::ios::out);
    this->dataFileStream.seekp(pageNumber * DATA_PAGE_SIZE);
    int recordsAmount = page->size();
    recordData dataToWrite[recordsAmount];
    for (int i = 0; i < recordsAmount; i++) {
        dataToWrite[i].a = page->at(i).getA();
        dataToWrite[i].b = page->at(i).getB();
        dataToWrite[i].h = page->at(i).getH();
        dataToWrite[i].key = page->at(i).getKey();
    }
    dataFileStream.write(reinterpret_cast<char*>(dataToWrite), DATA_RECORD_SIZE * recordsAmount);
    this->dataFileStream.close();
}

int DataManager::getlastDataPageNumber() {
    return this->lastDataPageNumber;
}

void DataManager::increaseLastDataPageNumber() {
    this->lastDataPageNumber++;
}

void DataManager::increaseDataPageRecordsCount() {
    this->dataPageRecordsCount++;
    if (this->dataPageRecordsCount >= DATA_PAGE_SIZE/DATA_RECORD_SIZE) {
        this->dataPageRecordsCount++;       // gdy strona się przepełni automatycznie przechodzimy na następną
    }
}

void DataManager::insertRecordToDiskPage(FileRecord *record, int pageNumber) {
    // CZY STRONA DYSKOWA Z REKORDAMI POWINNA BYĆ POSORTOWANA? NA RAZIE ZAKŁADAM ŻE NIE
    vector<FileRecord>* page = this->readRecordsDiskPage(pageNumber);
    page->push_back(*record);
    saveRecordsDiskPage(page, pageNumber);
    delete page;
}

FileRecord* DataManager::readRecordFromDiskPage(int key, int diskPageNumber) {
    vector<FileRecord>* page = this->readRecordsDiskPage(diskPageNumber);
    FileRecord* foundRecord = new FileRecord();
    for (int i = 0; i < page->size(); i++) {
        if (page->at(i).getKey() == key) {
            *foundRecord = page->at(i);
            break;
        }
    }
    delete page;
    return foundRecord;
}
