#include "BTreeRecord.h"

BTreeRecord::BTreeRecord() {
    this->key = 0;
    this->dataPageNumber = 0;
}

BTreeRecord::BTreeRecord(int key, int dataPageNumber) {
    this->key = key;
    this->dataPageNumber = dataPageNumber;
}

BTreeRecord::~BTreeRecord() {

}

int BTreeRecord::getKey() {
    return this->key;
}

void BTreeRecord::setKey(int key) {
    this->key = key;
}

int BTreeRecord::getPageNumberInFile() {
    return this->dataPageNumber;
}

void BTreeRecord::setDataPageNumberInFile(int dataPageNumber) {
    this->dataPageNumber = dataPageNumber;
}