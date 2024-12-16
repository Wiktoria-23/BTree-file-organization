#include "BTreePage.h"

#include <iostream>
#include <ostream>

BTreePage::BTreePage() {
    parentPageId = 0;
    records = new vector<BTreeRecord*>;
    childrensPagesIds = new vector<int>;
}

BTreePage::~BTreePage() {
    // chyba będzie trzeba jeszcze wyczyścić pamięć wektorów
    for (int i = 0; i < records->size(); i++) {
        delete records->at(i);
    }
    delete records;
    delete childrensPagesIds;
}

void BTreePage::setRecords(vector<BTreeRecord*>* newRecords) {
    records = newRecords;
}

vector<BTreeRecord*>* BTreePage::getRecords() {
    return records;
}
void BTreePage::setParentId(int newParentPageId) {
    parentPageId = newParentPageId;
}
int BTreePage::getParentId() {
    return parentPageId;
}
void BTreePage::setChildrenIds(vector<int>* newChildrensPagesIds) {
    childrensPagesIds = newChildrensPagesIds;
}
vector<int>* BTreePage::getChildrenIds() {
    return childrensPagesIds;
}

void BTreePage::setPageId(int newPageId) {
    pageId = newPageId;
}

int BTreePage::getPageId() {
    return pageId;
}

void BTreePage::addNewRecord(BTreeRecord* recordToAdd) {
    int insertIndex = findInsertIndex(recordToAdd->getKey());
    records->insert(records->begin() + insertIndex, recordToAdd);
}
void BTreePage::addNewChildId(int childId, int index) {
    childrensPagesIds->insert(childrensPagesIds->begin() + index, childId);
}

int BTreePage::searchKey(int key) {
    // funkcja zwraca numer elementu wektora, w którym znalazł się dany klucz lub numer elementu który może wskazywać na położenie w innym węźle
    int leftIndex = 0;
    int rightIndex = records->size() - 1;
    if (records->size() == 0 || records->at(leftIndex)->getKey() > key || records->at(rightIndex)->getKey() < key) {
        return -1;      // rekord nie znajduje się na stronie dyskowej
    }
    while (leftIndex < rightIndex) {
        int pivot = leftIndex + (rightIndex - leftIndex) / 2;
        int keyAtPivotElement = records->at(pivot)->getKey();
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
    if (records->at(leftIndex)->getKey() == key) {
        return leftIndex;
    }
    return -1;
}

int BTreePage::findChildIndex(int childId) {
    for (int i = 0; i < childrensPagesIds->size(); i++) {
        if (childrensPagesIds->at(i) == childId) {
            return i;
        }
    }
    return -1;      // teoretycznie nie powinno mieć miejsca
}

string BTreePage::toString() {
    string keys;
    for (int i = 0; i < records->size(); i++) {
        BTreeRecord* currentRecord = records->at(i);
        keys += ("(klucz: " + to_string(currentRecord->getKey()) + ", nr strony: " + to_string(currentRecord->getPageNumberInFile()) + "), ");
    }
    string childrenIds;
    for (int i = 0; i < childrensPagesIds->size(); i++) {
        childrenIds += to_string(childrensPagesIds->at(i)) + ", ";
    }
    return ("Wezel z id " + to_string(pageId) + "\nklucze:\n" + keys + "\nid dzieci: " + childrenIds + "\n");
}

int BTreePage::findInsertIndex(int key) {
    int leftIndex = 0;
    int rightIndex = records->size() - 1;
    if (records->size() == 0 || records->at(leftIndex)->getKey() > key) {
        return 0;      // umieszczamy rekord na początku strony dyskowej
    }
    if (records->at(rightIndex)->getKey() < key) {
        return records->size(); // umieszczamy rekord na końcu strony dyskowej
    }
    while (leftIndex < rightIndex) {
        int pivot = leftIndex + (rightIndex - leftIndex) / 2;
        int keyAtPivotElement = records->at(pivot)->getKey();
        if (key < keyAtPivotElement) {
            rightIndex = pivot;
        }
        else {
            leftIndex = pivot + 1;
        }
    }
    return leftIndex;
}

void BTreePage::printSortedKeys() {
    if (childrensPagesIds->empty()) {
        for (int i = 0; i < records->size(); i++) {
            // wyświetlenie znalezionego rekordu w pliku
        }
    }
    // tutaj musimy na przemian wyświetlać dzieci i klucze
}

