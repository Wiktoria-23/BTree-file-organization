#include "BTree.h"
#include "Program.h"

BTree::BTree(DataManager* newDataManager) {
    rootId = 0;  // rootId równe 0 oznacza, że nie mamy jeszcze głównego węzła w b drzewie, węzły są numerowane od 1
    d = BTREE_ORDER;
    dataManager = newDataManager;
}

BTree::~BTree() {

}

bool BTree::insertRecord(int freePageNumber, int key) {
    if (searchRecord(key) == 0) { // jeżeli zwróci zero to znaczy, że rekord nie został znaleziony
        int depth = dataManager->getVisitedPages()->size() - 1;
        BTreePage* page;
        // jeżeli lista odwiedzonych stron jest pusta, to znaczy, że jeszcze nie ma żadnych stron w programie
        if (dataManager->getVisitedPages()->empty()) {
            page = createNewRoot();
        }
        page = dataManager->getVisitedPages()->back();
        // dodajemy klucz do węzła
        BTreeRecord* recordToAdd = new BTreeRecord(key, freePageNumber);
        while (true) {
            if (page->getRecords()->size() < 2 * BTREE_ORDER) {
                page->addNewRecord(recordToAdd);
                dataManager->saveBTreePage(page, false, true);
                return true;
            }
            if (compensateNode(recordToAdd, depth)) {     // jeżeli się powiedzie to kończymy dodawanie klucza
                return true;
            }
            if (!splitNode(&recordToAdd, depth)) {
                depth -= 1; // jeżeli rozbijemy rodzica to wysokość drzewa się zmienia;
            }
            if (depth >= 0) {
                page = dataManager->getVisitedPages()->at(depth);
            }
            else {
                return true;
            }
        }
    }
    return false;
}

int BTree::searchRecord(int key) {
    // zwrócenie zera oznacza, że nie taki rekord nie istnieje w b drzewie
    if (rootId == 0) {
        return 0;    // nie ma jeszcze żadnego węzła
    }
    BTreePage* page;
    if (dataManager->getVisitedPages()->at(0)->getPageId() != rootId) {
        BTreePage* page = dataManager->loadBTreePage(rootId, true);
        dataManager->getVisitedPages()->insert(dataManager->getVisitedPages()->begin(), page);
    }
    else {
        page = dataManager->getVisitedPages()->at(0);
    }
    int depth = 0;
    while (true) {
        int index = page->searchKey(key);
        if (index != -1) {
            // szukany rekord został odnaleziony
            int pageId = page->getRecords()->at(index)->getPageNumberInFile();
            return pageId;
        }
        // jeżeli klucz nie znajduje się w węźle, w którym powinien się znajdować, to szukamy w dziecku
        index = page->findInsertIndex(key);
        vector<int>* childrenIds = page->getChildrenIds();
        if (static_cast<int>(childrenIds->size()) > index && dataManager->getVisitedPages()->size() > (depth + 1)) {
            int pageToReadId = childrenIds->at(index);
            if (dataManager->getVisitedPages()->at(depth + 1)->getPageId() != pageToReadId) {
                page = dataManager->loadBTreePage(pageToReadId, true);
                BTreePage* toDelete = dataManager->getVisitedPages()->at(depth + 1);
                dataManager->saveBTreePage(toDelete, true, true);
                dataManager->getVisitedPages()->erase(dataManager->getVisitedPages()->begin() + depth + 1);
                dataManager->getVisitedPages()->insert(dataManager->getVisitedPages()->begin() + depth + 1, page);
            }
            else {
                page = dataManager->getVisitedPages()->at(depth + 1);
            }
            depth++;
        }
        else {
            return 0;
        }
    }
}

BTreePage* BTree::createNewNode() {
    BTreePage* newNode = new BTreePage();
    newNode->setPageId(dataManager->getNextFreePageNumber());
    dataManager->increasePageNumber();
    newNode->setParentId(0);
    return newNode;
}

bool BTree::compensateNode(BTreeRecord* recordToAdd, int depth) {
    // jeżeli węzeł jest przepełniony to musimy równo rozdzielić klucze pomiędzy niego a sąsiada (compensation)
    bool left = true;
    if (depth < 1) {
        return false;
    }
    BTreePage* parent = dataManager->getVisitedPages()->at(depth - 1);  // pobieramy rodzica naszego obecnego węzła
    int index = parent->findChildIndex(dataManager->getVisitedPages()->at(depth)->getPageId());  // indeks klucza będzie ten sam, co indeks strony
    // znamy numer dziecka, które jest przepełnione
    BTreePage* leftSibling = nullptr;
    if (index - 1 >= 0) {
        leftSibling = dataManager->loadBTreePage(parent->getChildrenIds()->at(index - 1), true);
    }
    else {
        left = false;
    }
    BTreePage* leftNode;
    BTreePage* rightNode;
    // sprawdzamy, które z dzieci nie jest pełne
    if (leftSibling != nullptr && leftSibling->getRecords()->size() < 2 * BTREE_ORDER) {
        rightNode = dataManager->getVisitedPages()->at(depth);
        leftNode = leftSibling;
    }
    else {
        if (index + 1 < parent->getChildrenIds()->size()) {
            rightNode = dataManager->loadBTreePage(parent->getChildrenIds()->at(index + 1), true); // rodzeństwo z prawej strony
        }
        else {
            if (left && leftSibling->getPageId() != dataManager->getVisitedPages()->at(depth)->getPageId()) {
                delete leftSibling;
            }
            return false;
        }
        if (rightNode->getRecords()->size() < 2 * BTREE_ORDER) {
            leftNode = dataManager->getVisitedPages()->at(depth);
            rightNode = dataManager->loadBTreePage(parent->getChildrenIds()->at(index + 1), true);
        }
        else {
            if (left && leftSibling->getPageId() != dataManager->getVisitedPages()->at(depth)->getPageId()) {
                delete leftSibling;
            }
            if (rightNode->getPageId() != dataManager->getVisitedPages()->at(depth)->getPageId()) {
                delete rightNode;
            }
            return false;       // oba sąsiednie węzły są pełne - trzeba wykonać split
        }
    }
    distributeKeys(leftNode, rightNode, parent, &recordToAdd, depth, true);
    if (leftNode->getChildrenIds()->size() != 0 || rightNode->getChildrenIds()->size() != 0) {
        distributeChildren(leftNode, rightNode);
    }
    parent->addNewRecord(recordToAdd);
    dataManager->saveBTreePage(leftNode, false, true);
    dataManager->saveBTreePage(rightNode, false, true);
    dataManager->saveBTreePage(parent, false, true);
    if (left && leftSibling->getPageId() != dataManager->getVisitedPages()->at(depth)->getPageId()) {
        delete leftSibling;
    }
    return true;
}

bool BTree::splitNode(BTreeRecord** recordToAdd, int depth) {
    // tworzymy rodzeństwo dla węzła
    BTreePage* newPage = createNewNode();
    BTreePage* sibling = dataManager->getVisitedPages()->at(depth);
    BTreePage* parent;
    bool parentCreated = false;
    if (sibling->getParentId() == 0) {
        // musimy jeszcze utworzyć rodzica dla węzła, jeżeli go jeszcze nie ma
        parent = createNewRoot();
        sibling->setParentId(parent->getPageId());
        parent->addNewChildId(sibling->getPageId(), 0);
        parentCreated = true;
    }
    else {
        parent = dataManager->getVisitedPages()->at(depth - 1);
    }
    int splitingNodeIndex = parent->findChildIndex(sibling->getPageId());
    newPage->setParentId(sibling->getParentId());
    parent->addNewChildId(newPage->getPageId(), splitingNodeIndex);  // newPage jest lewym dzieckiem, więc dodajemy go przed rodzeństwem
    distributeKeys(newPage, sibling, parent, recordToAdd, depth, false);
    if (sibling->getChildrenIds()->size() != 0) {
        distributeChildren(newPage, sibling);
    }
    dataManager->saveBTreePage(newPage, true, true);
    dataManager->saveBTreePage(sibling, false, true);
    dataManager->saveBTreePage(parent, false, true);
    return parentCreated;
}

void BTree::distributeKeys(BTreePage* leftSibling, BTreePage* rightSibling, BTreePage* parent, BTreeRecord** recordToAdd, int depth, bool changeParentNode) {
    int index;
    // jeżeli jesteśmy przy rozbijaniu węzła, to rodzic może nie posiadać jeszcze żadnych rekordów
    if (!parent->getRecords()->empty()) {
        index = parent->findChildIndex(leftSibling->getPageId());
    }
    else {
        index = 0;
    }
    vector<BTreeRecord*> recordsToDistribute;
    // stworzenie listy ze wszystkimi rekordami do dystrybucji
    recordsToDistribute.insert(
        recordsToDistribute.end(),
        leftSibling->getRecords()->begin(),
        leftSibling->getRecords()->end());
    if (changeParentNode && !parent->getRecords()->empty()) {
        recordsToDistribute.insert(recordsToDistribute.end(), parent->getRecords()->at(index));
        parent->getRecords()->erase(parent->getRecords()->begin() + index);
    }
    recordsToDistribute.insert(
        recordsToDistribute.end(),
        rightSibling->getRecords()->begin(),
        rightSibling->getRecords()->end());
    // usunięcie pobranych rekordów z dzieci
    leftSibling->getRecords()->clear();
    rightSibling->getRecords()->clear();
    for (int i = 0; i < recordsToDistribute.size() + 1; i++) {
        if (i >= recordsToDistribute.size()) {
            recordsToDistribute.push_back(*recordToAdd);
            break;
        }
        if ((*recordToAdd)->getKey() < recordsToDistribute[i]->getKey()) {
            if (i > 1) {
                recordsToDistribute.insert(recordsToDistribute.begin() + i, *recordToAdd);
                break;
            }
            recordsToDistribute.insert(recordsToDistribute.begin(), *recordToAdd);
            break;
        }
    }
    // ponowne dodanie rekordów, tym razem w odpowiednich ilościach na poszczególnych węzłach
    leftSibling->getRecords()->insert(
        leftSibling->getRecords()->end(),
        recordsToDistribute.begin(),
        recordsToDistribute.begin() + (recordsToDistribute.size() / 2));
    // funkcja podmienia wartość recordsToAdd by zwrócić rekord do dodania do rodzica
    *recordToAdd = recordsToDistribute.at(recordsToDistribute.size() / 2);
    rightSibling->getRecords()->insert(
        rightSibling->getRecords()->end(),
        recordsToDistribute.begin() + ((recordsToDistribute.size() / 2) + 1),
        recordsToDistribute.end());
}

int BTree::getRootId() {
    return rootId;
}

BTreePage* BTree::createNewRoot() {
    BTreePage* page = new BTreePage();
    page->setPageId(this->dataManager->getNextFreePageNumber());
    dataManager->increasePageNumber();
    rootId = page->getPageId();
    dataManager->getVisitedPages()->insert(dataManager->getVisitedPages()->begin(), page);
    return page;
}

void BTree::distributeChildren(BTreePage *leftSibling, BTreePage *rightSibling) {
    vector<int> childIds;
    childIds.insert(childIds.begin(), leftSibling->getChildrenIds()->begin(), leftSibling->getChildrenIds()->end());
    childIds.insert(childIds.end(), rightSibling->getChildrenIds()->begin(), rightSibling->getChildrenIds()->end());
    leftSibling->getChildrenIds()->clear();
    rightSibling->getChildrenIds()->clear();
    int leftSiblingChildrenNumber = leftSibling->getRecords()->size() + 1;
    leftSibling->getChildrenIds()->insert(leftSibling->getChildrenIds()->begin(), childIds.begin(), childIds.begin() + leftSiblingChildrenNumber);
    rightSibling->getChildrenIds()->insert(rightSibling->getChildrenIds()->begin(), childIds.begin() + leftSiblingChildrenNumber, childIds.end());
}

bool BTree::searchForRecord(int key) {
    int recordPage = searchRecord(key);
    if (recordPage != 0) {
        // odczytaj rekord ze strony
        FileRecord* foundRecord = dataManager->readRecordFromDiskPage(key, recordPage, true);
        cout << foundRecord->toString() << endl;
        delete foundRecord;
        return true;
    }
    cout << "Rekord o podanym kluczu nie zostal odnaleziony" << endl;
    return false;
}

void BTree::printSortedData() {
    BTreePage* root = dataManager->loadBTreePage(rootId, false);
    if (root != nullptr) {
        printSortedNode(root);
    }
}

void BTree::printSortedNode(BTreePage* node) {
    if (node->getChildrenIds()->empty()) {
        for (int i = 0; i < node->getRecords()->size(); i++) {
            printDataRecord(node, i);
        }
        return;
    }
    BTreePage* child;
    for (int i = 0; i < node->getRecords()->size(); i++) {
        child = dataManager->loadBTreePage(node->getChildrenIds()->at(i), false);// załadowanie dziecka
        printSortedNode(child);
        printDataRecord(node, i);
    }
    child = dataManager->loadBTreePage(node->getChildrenIds()->back(), false);// załadowanie dziecka
    printSortedNode(child);
}

void BTree::printDataRecord(BTreePage* node, int index) {
    BTreeRecord* record = node->getRecords()->at(index);
    FileRecord* data = dataManager->readRecordFromDiskPage(record->getKey(), record->getPageNumberInFile(), false);
    cout << data->toString() << endl;
    delete data;
}
