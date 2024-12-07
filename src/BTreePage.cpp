#include "BTreePage.h"

BTreePage::BTreePage() {
    this->parentPageId = 0;
    this->records = new vector<BTreeRecord>;
    this->childrensPagesIds = new vector<int>;
    this->keysNumber = 0;
    this->childrenNumber = 0;
}

BTreePage::~BTreePage() {
    // chyba będzie trzeba jeszcze wyczyścić pamięć wektorów
    delete this->records;
    this->deleteChildren();
}

void BTreePage::deleteChildren() {
    // tutaj to powinno być zmienione na wersję z ładowaniem stron
    /*// rekurencyjne usuwanie dzieci węzła
    if (this->children->size() > 0) {
        for (BTreePage* child : this->children) {
            child->deleteChildren();
            delete child;
        }
    }
    delete this->children;*/
}

void BTreePage::setKeysNumber(int keysNumber) {
    this->keysNumber = keysNumber;
}

int BTreePage::getKeysNumber() {
    return this->keysNumber;
}

void BTreePage::setRecords(vector<BTreeRecord>* records) {
    this->records = records;
}

vector<BTreeRecord>* BTreePage::getRecords() {
    return this->records;
}
void BTreePage::setParentId(int parentPageId) {
    this->parentPageId = parentPageId;
}
int BTreePage::getParentId() {
    return this->parentPageId;
}
void BTreePage::setChildrenIds(vector<int>* childrensPagesIds) {
    this->childrensPagesIds = childrensPagesIds;
}
vector<int>* BTreePage::getChildrenIds() {
    return this->childrensPagesIds;
}

void BTreePage::setPageId(int pageId) {
    this->pageId = pageId;
}

int BTreePage::getPageId() {
    return this->pageId;
}

void BTreePage::addNewRecord(int key, int dataPageNumber) {
    BTreeRecord* record = new BTreeRecord;
    record->setKey(key);
    record->setDataPageNumberInFile(dataPageNumber);
    int insertIndex = this->searchKey(key); // szukamy indeksu, by umieścić klucz w odpowiednim miejscu
    this->records->insert(this->records->begin() + insertIndex, *record);
    delete record;
    keysNumber++;
}
void BTreePage::addNewChildId(int childId) {
    this->childrensPagesIds->push_back(childId);
}

int BTreePage::searchKey(int key) {
    // funkcja zwraca numer elementu wektora, w którym znalazł się dany klucz lub numer elementu który może wskazywać na położenie w innym węźle
    int leftIndex = 0;
    int rightIndex = this->records->size()-1;
    if (this->records->size() == 0) {
        return 0;
    }
    if (this->records->at(leftIndex).getKey() > key) {
        return 0;
    }
    else if (this->records->at(rightIndex).getKey() < key) {
        return this->records->size();
    }
    while (leftIndex < rightIndex) {
        int pivot = leftIndex + (rightIndex - leftIndex) / 2;
        int keyAtPivotElement = this->records->at(pivot).getKey();
        if (key < keyAtPivotElement) {
            rightIndex = pivot;
        }
        else if (key == keyAtPivotElement) {
            return pivot;
        }
        else {
            leftIndex = pivot + 1;
        }
    }
    return leftIndex;
}

int BTreePage::getChildrenNumber() {
    return this->childrenNumber;
}

void BTreePage::setChildrenNumber(int childrenNumber) {
    this->childrenNumber = childrenNumber;
}