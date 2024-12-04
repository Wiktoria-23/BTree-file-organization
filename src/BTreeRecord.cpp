#include "BTreeRecord.h"

BTreeRecord::BTreeRecord() {
    this->key = 0;
    this->addressInFile = 0;
}

BTreeRecord::BTreeRecord(unsigned int key, unsigned int addressInFile) {
    this->key = key;
    this->addressInFile = addressInFile;
}

BTreeRecord::~BTreeRecord() {

}
