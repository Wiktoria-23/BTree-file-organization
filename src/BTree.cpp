#include "BTree.h"
#include "Program.h"

BTree::BTree(DataManager* newDataManager) {
    rootId = 0;  // rootId równe 0 oznacza, że nie mamy jeszcze głównego węzła w b drzewie, węzły są numerowane od 1
    d = BTREE_ORDER;
    dataManager = newDataManager;
}

BTree::~BTree() {
    // TODO: NAPRAW DESTRUKTOR
    /*currentPage->loadBTreePage(*rootAddress);
    currentPage->deleteChildren();
    delete rootAddress;*/
}

bool BTree::insertRecord(int freePageNumber, int key) {
    if (searchRecord(key) == 0) { // jeżeli zwróci zero to znaczy, że rekord nie został znaleziony
        int depth = visitedPages.size() - 1;
        BTreePage* page;
        // jeżeli lista odwiedzonych stron jest pusta, to znaczy, że jeszcze nie ma żadnych stron w programie
        if (visitedPages.empty()) {
            page = createNewRoot();
        }
        page = visitedPages.back();
        // dodajemy klucz do węzła
        BTreeRecord* recordToAdd = new BTreeRecord(key, freePageNumber);
        while (true) {
            if (page->getRecords()->size() < 2 * BTREE_ORDER) {
                page->addNewRecord(recordToAdd);
                dataManager->saveBTreePage(page);
                // TODO: ZMIEŃ BY ZAPISYWAĆ STRONĘ DOPIERO GDY MUSIMY JĄ PODMIENIĆ W BUFORZE
                return true;
            }
            if (compensateNode(recordToAdd, depth)) {     // jeżeli się powiedzie to kończymy dodawanie klucza
                return true;
            }
            splitNode(recordToAdd, depth);
            depth -= 1;
            if (visitedPages.empty()) {
                return true;
            }
            page = visitedPages.back();
            if (page->getRecords()->size() < 2 * BTREE_ORDER) {
                return true;
            }
            depth -= 1;
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
    if (visitedPages.at(0)->getPageId() != rootId) {
        BTreePage* page = dataManager->loadBTreePage(rootId);
        delete (visitedPages.at(0));
        visitedPages.erase(visitedPages.begin());
        visitedPages.insert(visitedPages.begin(), page);
    }
    else {
        page = visitedPages.at(0);
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
        vector<int>* childrenId = page->getChildrenIds();
        // index nie może być zmniejszony o 1!, a ten drugi warunek jest chyba ninepotrzebny
        if (static_cast<int>(childrenId->size()) > index && visitedPages.size() > (depth + 1)) {
            int pageToReadId = childrenId->at(index);
            if (visitedPages.at(depth + 1)->getPageId() != pageToReadId) {
                page = dataManager->loadBTreePage(pageToReadId);
                visitedPages.push_back(page);
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
    BTreePage* parent = visitedPages.at(depth - 1);  // pobieramy rodzica naszego obecnego węzła
    int index = parent->findChildIndex(visitedPages.at(depth)->getPageId());  // indeks klucza będzie ten sam, co indeks strony
    // znamy numer dziecka, które jest przepełnione
    BTreePage* leftSibling;
    if (index - 1 >= 0) {
        leftSibling = dataManager->loadBTreePage(parent->getChildrenIds()->at(index - 1));
    }
    else {
        left = false;
    }
    BTreePage* leftNode;
    BTreePage* rightNode;
    // sprawdzamy, które z dzieci nie jest pełne
    if (leftSibling != nullptr && leftSibling->getRecords()->size() < 2 * BTREE_ORDER) {
        rightNode = visitedPages.at(depth);
        leftNode = leftSibling;
    }
    else {
        if (index + 1 < parent->getChildrenIds()->size()) {
            rightNode = dataManager->loadBTreePage(parent->getChildrenIds()->at(index + 1)); // rodzeństwo z prawej strony
        }
        else {
            if (left) {
                delete leftSibling;
            }
            return false;
        }
        if (rightNode->getRecords()->size() < 2 * BTREE_ORDER) {
            leftNode = visitedPages.at(depth);
            rightNode = dataManager->loadBTreePage(parent->getChildrenIds()->at(index + 1));
        }
        else {
            if (left) {
                delete leftSibling;
            }
            delete rightNode;
            return false;       // oba sąsiednie węzły są pełne - trzeba wykonać split
        }
    }
    distributeKeys(leftNode, rightNode, parent, recordToAdd, depth, true);
    dataManager->saveBTreePage(leftNode);
    dataManager->saveBTreePage(rightNode);
    dataManager->saveBTreePage(parent);
    if (left) {
        delete leftSibling;
    }
    return true;
}

void BTree::splitNode(BTreeRecord* recordToAdd, int depth) {
    // tworzymy rodzeństwo dla węzła
    BTreePage* newPage = createNewNode();
    BTreePage* sibling = visitedPages.at(depth);
    BTreePage* parent;
    if (sibling->getParentId() == 0) {
        // musimy jeszcze utworzyć rodzica dla węzła, jeżeli go jeszcze nie ma
        parent = createNewRoot();
        sibling->setParentId(parent->getPageId());
        parent->addNewChildId(sibling->getPageId(), 0);
    }
    else {
        parent = visitedPages.at(depth - 1);
    }
    int splitingNodeIndex = parent->findChildIndex(sibling->getPageId());
    newPage->setParentId(sibling->getParentId());
    parent->addNewChildId(newPage->getPageId(), splitingNodeIndex);  // newPage jest lewym dzieckiem, więc dodajemy go przed rodzeństwem
    distributeKeys(newPage, sibling, parent, recordToAdd, depth, false);
    dataManager->increasePageNumber();
    dataManager->saveBTreePage(newPage);
    dataManager->saveBTreePage(sibling);
    dataManager->saveBTreePage(parent);
}

void BTree::distributeKeys(BTreePage* leftSibling, BTreePage* rightSibling, BTreePage* parent, BTreeRecord* recordToAdd, int depth, bool changeParentNode) {
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
            recordsToDistribute.push_back(recordToAdd);
            break;
        }
        if (recordToAdd->getKey() < recordsToDistribute[i]->getKey()) {
            if (i > 1) {
                recordsToDistribute.insert(recordsToDistribute.begin() + i - 1, recordToAdd);
                break;
            }
            else {
                recordsToDistribute.insert(recordsToDistribute.begin(), recordToAdd);
                break;
            }
        }
    }
    // ponowne dodanie rekordów, tym razem w odpowiednich ilościach na poszczególnych węzłach
    leftSibling->getRecords()->insert(
        leftSibling->getRecords()->end(),
        recordsToDistribute.begin(),
        recordsToDistribute.begin() + (recordsToDistribute.size() / 2));
    parent->getRecords()->insert(
        parent->getRecords()->begin() + index,
        recordsToDistribute.at(recordsToDistribute.size() / 2));
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
    visitedPages.insert(visitedPages.begin(), page);
    return page;
}
