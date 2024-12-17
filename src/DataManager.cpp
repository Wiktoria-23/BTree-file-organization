#include "BTreePage.h"
#include "BTreeRecord.h"
#include "dataManager.h"
#include "Program.h"

DataManager::DataManager() {
    dataFilename = "../data.dat";
    bTreeFilename = "../bTreeData.dat";
    resetFiles();
    lastDataPageNumber = 1;
    dataPageRecordsCount = 0;
    nextFreeBTreePageNumber = 1;  // disk page numbers start from 1
    visitedPages = new vector<BTreePage*>;
    diskPageReads = 0;
    diskPageWrites = 0;
}

DataManager::~DataManager() {
    if (bTreeFileStream.is_open()) {
        bTreeFileStream.close();
    }
    if (dataFileStream.is_open()) {
        dataFileStream.close();
    }
    for (int i = 0; i < visitedPages->size(); i++) {
        delete visitedPages->at(i);
    }
    delete visitedPages;
}

void DataManager::resetFile(string filename, fstream &fileStream) {
    fileStream.open(filename, std::ios::binary | std::ios::out);
    fileStream.close();
}

BTreePage* DataManager::loadBTreePage(int pageNumber, bool countDiskAccess) {
    if (pageNumber == 0) {
        return nullptr;
    }
    // if page is already read to memory, we get its data from visited pages buffer instead of reading file
    for (int i = 0; i < visitedPages->size(); i++) {
        if (visitedPages->at(i)->getPageId() == pageNumber) {
            return visitedPages->at(i);
        }
    }
    if (countDiskAccess) {
        diskPageReads++;
    }
    bTreeFileStream.open(bTreeFilename, std::ios::binary | std::ios::in);
    // b tree nodes are numbered from 1, so address in file must be decreased
    bTreeFileStream.seekg(BTREE_PAGE_SIZE * (pageNumber - 1));
    int bTreePageData[BTREE_PAGE_SIZE/sizeof(int)];
    bTreeFileStream.read(reinterpret_cast<char*>(bTreePageData), BTREE_PAGE_SIZE);
    BTreePage* pageRead = new BTreePage();
    // reading heading from b tree data page
    pageRead->setPageId(bTreePageData[0]);
    pageRead->setParentId(bTreePageData[2]);
    int childrenNumber = bTreePageData[3];
    // reading b tree node data
    int keysNumber = bTreePageData[1];
    // reading keys and children from saved data
    for (int i = 0; i < keysNumber; i++) {
        BTreeRecord* recordToAdd = new BTreeRecord(
            bTreePageData[4 + 2 * i],
            bTreePageData[4 + 2 * i + 1]);
        pageRead->addNewRecord(recordToAdd);
    }
    vector<int>* childrensIds = pageRead->getChildrenIds();
    int offset = 4 + BTREE_ORDER * 4;                   // offset to children ids
    for (int i = 0; i < childrenNumber; i++) {  // node has one child more than keys
        childrensIds->push_back(bTreePageData[offset + i]);
    }
    bTreeFileStream.close();
    return pageRead;
}

void DataManager::saveBTreePage(BTreePage *page, bool deleting, bool countDiskAccess) {
    // if b tree page is in visited pages buffer we don't save it to file
    // b tree page is saved when it is deleted from visite pages buffer
    if (!deleting) {
        for (int i = 0; i < visitedPages->size(); i++) {
            if (visitedPages->at(i)->getPageId() == page->getPageId()) {
                return;
            }
        }
    }
    if (countDiskAccess) {
        diskPageWrites++;
    }
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
    // calculate offset to beginning of children ids
    int offset = 4 + BTREE_ORDER * 4;
    // adding 4, because header is 4 bytes long
    // b tree order is multiplied by 4 as node can contain 2*order records, two ints each
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

vector<FileRecord>* DataManager::readRecordsDiskPage(int diskPageNumber, bool countDiskAccess) {
    if (countDiskAccess) {
        diskPageReads++;
    }
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

void DataManager::saveRecordsDiskPage(vector<FileRecord>* page, int pageNumber, bool countDiskAccess) {
    if (countDiskAccess) {
        diskPageWrites++;
    }
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
        lastDataPageNumber++;       // if page is full we increase last page number
        dataPageRecordsCount = 0;
    }
}

void DataManager::insertRecordToDiskPage(FileRecord *record, int pageNumber) {
    vector<FileRecord>* page = readRecordsDiskPage(pageNumber, true);
    page->push_back(*record);
    saveRecordsDiskPage(page, pageNumber, true);
    delete page;
}

FileRecord* DataManager::readRecordFromDiskPage(int key, int diskPageNumber, bool countDiskAccess) {
    vector<FileRecord>* page = readRecordsDiskPage(diskPageNumber, countDiskAccess);
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
    vector<FileRecord>* page = readRecordsDiskPage(pageNumber, true);
    int index;
    for (int i = 0; i < page->size(); i++) {
        if (page->at(i).getKey() == record->getKey()) {
            index = i;
            break;
        }
    }
    page->erase(page->begin() + index);
    page->insert(page->begin()+ index, *record);
    saveRecordsDiskPage(page, pageNumber, true);
    delete page;
}

vector<BTreePage*>* DataManager::getVisitedPages() {
    return visitedPages;
}

void DataManager::resetFiles() {
    resetFile(this->bTreeFilename, this->bTreeFileStream);
    resetFile(this->dataFilename, this->dataFileStream);
}

void DataManager::resetDiskAccesses() {
    diskPageReads = 0;
    diskPageWrites = 0;
}

int DataManager::getDiskPageReadsNumber() {
    return diskPageReads;
}

int DataManager::getDiskPageWritesNumber() {
    return diskPageWrites;
}
