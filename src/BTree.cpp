#include "BTree.h"
#include "Program.h"

BTree::BTree(DataManager* dataManager) {
    rootId = 0;  // rootId równe 0 oznacza, że nie mamy jeszcze głównego węzła w b drzewie, węzły są numerowane od 1
    d = BTREE_ORDER;
    nodesCount = 0;
    this->dataManager = dataManager;
    currentPage = nullptr;
}

BTree::~BTree() {
    /*currentPage->loadBTreePage(*rootAddress);
    currentPage->deleteChildren();
    delete rootAddress;*/
}

bool BTree::insertRecord(int freePageNumber, int key) {
    if (this->searchRecord(key) == nullptr) {
        // jeżeli obecna strona wskazuje na nullptr to znaczy, że nie mamy jeszcze żadnej w programie
        if (this->currentPage == nullptr) {
            this->currentPage = new BTreePage();
            this->currentPage->setPageId(this->dataManager->getNextFreePageNumber());
            this->dataManager->increaseDataPageRecordsCount();
            this->rootId = this->currentPage->getPageId();
        }
        if (this->currentPage->getKeysNumber() < 2 * BTREE_ORDER) {
            this->currentPage->addNewRecord(key, freePageNumber);
            this->dataManager->saveBTreePage(currentPage);
            return true;
        }
        else if () {
            // compensation
            return true;
        }
        else {
            // split
            // PO TYM TRZEBA KONTYNUOWAĆ W GÓRĘ DRZEWA CHYBA
        }
        return true;
    }
    cout << "Rekord o podanym kluczu juz istnieje" << endl;
    return false;
}

FileRecord* BTree::searchRecord(int key) {
    if (this->rootId == 0) {
        return nullptr;    // nie ma jeszcze żadnego węzła
    }
    this->currentPage = this->dataManager->loadBTreePage(rootId);
    while (true) {
        int index = this->currentPage->searchKey(key);
        if (this->currentPage->getRecords()->at(index).getKey() == key) {
            // szukany rekord został odnaleziony
            int pageId = this->currentPage->getRecords()->at(index).getPageNumberInFile();
            return this->dataManager->readRecordFromDiskPage(key, pageId);
        }
        if (this->currentPage->getRecords()->at(index).getKey() < key) {
            // szukany rekord znajduje się w lewym dziecku, o ile takie istnieje
            vector<int>* childrenId = this->currentPage->getChildrenIds();
            if (childrenId->size() > index - 1) {
                this->currentPage = this->dataManager->loadBTreePage(childrenId->at(index-1));
            }
            return nullptr;
            // sprawdź czy dobrze pobierasz id dzieci
        }
        if (this->currentPage->getRecords()->at(index).getKey() > key) {
            // szukany rekord znajduje się w prawym dziecku, o ile takie istnieje
            vector<int>* childrenId = this->currentPage->getChildrenIds();
            if (childrenId->size() > index) {
                this->currentPage = this->dataManager->loadBTreePage(childrenId->at(index));
            }
            return nullptr;
            // tutaj tak samo
        }
    }
}

void BTree::updateRecord(int key) {
    // to będzie wyszukanie rekordu - z użyciem funkcji search, a następnie nadpisanie całej strony dyskowej
}

BTreePage* BTree::createNewNode() {
    // zawsze gdy tworzymy nowy węzeł to jest to nowy root
    BTreePage* newNode = new BTreePage();
    newNode->setPageId(1);
    this->nodesCount++;
    newNode->setKeysNumber(1);
    newNode->setParentId(0);
    return newNode;
}
