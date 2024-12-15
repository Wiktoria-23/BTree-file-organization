#include "BTreePage.h"
#include "BTreeRecord.h"
#include "dataManager.h"
#include "Program.h"

DataManager::DataManager() {
    dataFilename = "../data.dat";
    bTreeFilename = "../bTreeData.dat";
    ensureFileExist(this->bTreeFilename, this->bTreeFileStream);
    ensureFileExist(this->dataFilename, this->dataFileStream);
    lastDataPageNumber = 1;
    dataPageRecordsCount = 0;
    nextFreeBTreePageNumber = 1;  // strony dla węzłów adresujemy od 1, by łatwiej uwzględniać brak rodzica
    visitedPages = new vector<BTreePage*>;
}

DataManager::~DataManager() {
    if (bTreeFileStream.is_open()) {
        bTreeFileStream.close();
    }
    if (dataFileStream.is_open()) {
        dataFileStream.close();
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

BTreePage* DataManager::loadBTreePage(int pageNumber) {
    if (pageNumber == 0) {
        return nullptr;
    }
    bTreeFileStream.open(bTreeFilename, std::ios::binary | std::ios::in);
    bTreeFileStream.seekg(BTREE_PAGE_SIZE * (pageNumber - 1)); // strony są numerowane od 1, więc musimy zmniejszyć adres początkowy
    int bTreePageData[BTREE_PAGE_SIZE/sizeof(int)];
    bTreeFileStream.read(reinterpret_cast<char*>(bTreePageData), BTREE_PAGE_SIZE);
    BTreePage* pageRead = new BTreePage();
    // odczytanie nagłówka strony b drzewa
    pageRead->setPageId(bTreePageData[0]);
    pageRead->setParentId(bTreePageData[2]);
    int childrenNumber = bTreePageData[3];
    // odczytanie danych węzła b drzewa
    int keysNumber = bTreePageData[1];
    for (int i = 0; i < keysNumber; i++) {          // wpisujemy do wektora tylko tyle kluczy, ile rzeczywiście zawiera węzeł b drzewa
        BTreeRecord* recordToAdd = new BTreeRecord(bTreePageData[4 + 2 * i], bTreePageData[4 + 2 * i + 1]);
        pageRead->addNewRecord(recordToAdd);
    }
    vector<int>* childrensIds = pageRead->getChildrenIds();
    int offset = 4 + BTREE_ORDER * 4;                   // offset do miejsca w którym zaczynamy czytać id węzłów potomnych
    for (int i = 0; i < childrenNumber; i++) {  // węzeł będzie mieć zawsze o 1 więcej węzłów potomnych niż kluczy
        childrensIds->push_back(bTreePageData[offset + i]);
    }
    bTreeFileStream.close();
    return pageRead;
    // NIE WIEM CZY POPRAWNIE BĘDĄ SIĘ DODAWAĆ ID WĘZŁÓW POTOMNYCH TUTAJ, ALE CHYBA POWINNY
}

void DataManager::saveBTreePage(BTreePage *page) {
    bTreeFileStream.open(bTreeFilename, std::ios::binary | std::ios::in | std::ios::out);
    bTreeFileStream.seekp((page->getPageId() - 1) * BTREE_PAGE_SIZE);
    int bTreePageData[BTREE_PAGE_SIZE/sizeof(int)];
    bTreePageData[0] = page->getPageId();
    bTreePageData[1] = page->getRecords()->size();
    bTreePageData[2] = page->getParentId();
    bTreePageData[3] = page->getChildrenIds()->size();
    vector<BTreeRecord*>* records = page->getRecords();
    for (int i = 0; i < records->size(); i++) {
        bTreePageData[4 + 2 * i] = records->at(i)->getKey();
        bTreePageData[4 + 2 * i + 1] = records->at(i)->getPageNumberInFile();
    }
    // przesunięcie wskaźnika poza nagłówek i sekcję, gdzie zostają zapisane klucze wraz z ich adresami
    int offset = 4 + BTREE_ORDER * 4;
    // 3 to długość nagłówka, mnożymy przez cztery, bo maks liczba kluczy to 2d i każdy rekord zajmuje dwie zmienne typu int
    vector<int>* childrensIds = page->getChildrenIds();
    for (int i = 0; i < childrensIds->size(); i++) {
        bTreePageData[offset + i] = childrensIds->at(i);
    }
    bTreeFileStream.write(reinterpret_cast<char*>(bTreePageData), BTREE_PAGE_SIZE);
    bTreeFileStream.close();
}

int DataManager::getNextFreePageNumber() {
    return nextFreeBTreePageNumber;
}

void DataManager::increasePageNumber() {
    nextFreeBTreePageNumber += 1;
}

vector<FileRecord>* DataManager::readRecordsDiskPage(int diskPageNumber) {
    dataFileStream.open(dataFilename, std::ios::binary | std::ios::in);
    dataFileStream.seekg((diskPageNumber - 1) * DATA_PAGE_SIZE);
    recordData recordsData[DATA_PAGE_SIZE/DATA_RECORD_SIZE];
    dataFileStream.read(reinterpret_cast<char*>(recordsData), DATA_PAGE_SIZE);
    int recordsRead = dataFileStream.gcount() / DATA_RECORD_SIZE;
    vector<FileRecord>* records = new vector<FileRecord>();
    for (int i = 0; i < recordsRead; i++) {
        FileRecord record = FileRecord(recordsData[i].a, recordsData[i].b, recordsData[i].h, recordsData[i].key);
        records->push_back(record);
    }
    dataFileStream.close();
    return records;
}

void DataManager::saveRecordsDiskPage(vector<FileRecord>* page, int pageNumber) {
    dataFileStream.open(dataFilename, std::ios::binary | std::ios::in | std::ios::out);
    dataFileStream.seekp((pageNumber - 1) * DATA_PAGE_SIZE);
    int recordsNumber = page->size();
    recordData dataToWrite[DATA_PAGE_SIZE/DATA_RECORD_SIZE];
    for (int i = 0; i < recordsNumber; i++) {
        dataToWrite[i].a = page->at(i).getA();
        dataToWrite[i].b = page->at(i).getB();
        dataToWrite[i].h = page->at(i).getH();
        dataToWrite[i].key = page->at(i).getKey();
    }
    dataFileStream.write(reinterpret_cast<char*>(dataToWrite), DATA_RECORD_SIZE * recordsNumber);
    dataFileStream.close();
}

int DataManager::getlastDataPageNumber() {
    return lastDataPageNumber;
}

void DataManager::increaseDataPageRecordsCount() {
    dataPageRecordsCount++;
    if (dataPageRecordsCount >= DATA_PAGE_SIZE/DATA_RECORD_SIZE) {
        lastDataPageNumber++;       // gdy strona się przepełni automatycznie przechodzimy na następną
        dataPageRecordsCount = 0;
    }
}

void DataManager::insertRecordToDiskPage(FileRecord *record, int pageNumber) {
    // CZY STRONA DYSKOWA Z REKORDAMI POWINNA BYĆ POSORTOWANA? NA RAZIE ZAKŁADAM ŻE NIE
    vector<FileRecord>* page = readRecordsDiskPage(pageNumber);
    page->push_back(*record);
    saveRecordsDiskPage(page, pageNumber);
    delete page;
}

FileRecord* DataManager::readRecordFromDiskPage(int key, int diskPageNumber) {
    vector<FileRecord>* page = readRecordsDiskPage(diskPageNumber);
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

void DataManager::updateRecordOnDiskPage(FileRecord *record, int pageNumber) {
    vector<FileRecord>* page = readRecordsDiskPage(pageNumber);
    int index;
    for (int i = 0; i < page->size(); i++) {
        if (page->at(i).getKey() == record->getKey()) {
            index = i;
            break;
        }
    }
    page->erase(page->begin() + index);
    page->insert(page->begin()+ index, *record);
    saveRecordsDiskPage(page, pageNumber);
    delete page;
}

vector<BTreePage*>* DataManager::getVisitedPages() {
    return visitedPages;
}
