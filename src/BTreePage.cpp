#include "BTreePage.h"

#include <iostream>
#include <ostream>

BTreePage::BTreePage() {
    parentPageId = 0;
    records = new vector<BTreeRecord*>;
    childrensPagesIds = new vector<int>;
}

BTreePage::~BTreePage() {
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
    // function returns number of element in node that contains key or number of child which should contain it
    int leftIndex = 0;
    int rightIndex = records->size() - 1;
    if (records->size() == 0 || records->at(leftIndex)->getKey() > key || records->at(rightIndex)->getKey() < key) {
        return -1;      // this record can't be on current disk page
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
    return -1;
}

string BTreePage::toString() {
    string keys;
    for (int i = 0; i < records->size(); i++) {
        BTreeRecord* currentRecord = records->at(i);
        keys += ("(klucz: " + to_string(currentRecord->getKey())
            + ", nr strony: " + to_string(currentRecord->getPageNumberInFile()) + "), ");
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
        return 0;      // record should be inserted at the beginning of the disk page
    }
    if (records->at(rightIndex)->getKey() < key) {
        return records->size(); // record should be inserted at the end of disk page
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
