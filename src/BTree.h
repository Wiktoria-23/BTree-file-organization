#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include "BTreePage.h"
#include "DataManager.h"
#include "FileRecord.h"
using namespace std;

class BTree {
private:
    BTreePage *currentPage;
    DataManager* dataManager;
    int rootId;
    int d;
    int nodesCount;
public:
    BTree(DataManager* dataManager);
    ~BTree();
    bool insertRecord(int freePageNumber, int key);
    void updateRecord(int key);
    FileRecord* searchRecord(int key);
    BTreePage* createNewNode();
};



#endif //BTREE_H
