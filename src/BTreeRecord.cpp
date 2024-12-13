#include "BTreeRecord.h"

BTreeRecord::BTreeRecord() {
    key = 0;
    dataPageNumber = 0;
}

BTreeRecord::BTreeRecord(int newKey, int newDataPageNumber) {
    key = newKey;
    dataPageNumber = newDataPageNumber;
}

BTreeRecord::~BTreeRecord() {

}

int BTreeRecord::getKey() {
    return key;
}

void BTreeRecord::setKey(int newKey) {
    key = newKey;
}

int BTreeRecord::getPageNumberInFile() {
    return dataPageNumber;
}

void BTreeRecord::setDataPageNumberInFile(int newDataPageNumber) {
    dataPageNumber = newDataPageNumber;
}