#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include "BTreePage.h"
#include "FileRecord.h"
using namespace std;


class BTree {
private:
    BTreePage *currentPage;
    unsigned int* rootAddress;
    unsigned int d;
public:
    BTree();
    ~BTree();
    bool insertRecord();
    void updateRecord(unsigned int key);
    FileRecord* searchRecord(unsigned int key);
};



#endif //BTREE_H
