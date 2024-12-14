#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include "BTreePage.h"
#include "DataManager.h"
#include "FileRecord.h"
using namespace std;

class BTree {
private:
    vector<BTreePage*> visitedPages;
    DataManager* dataManager;
    int rootId;
    int d;
public:
    BTree(DataManager* newDataManager);
    ~BTree();
    bool insertRecord(int freePageNumber, int key);
    int searchRecord(int key);
    BTreePage* createNewNode();
    bool compensateNode(BTreeRecord* recordToAdd, int depth);
    bool splitNode(BTreeRecord** recordToAdd, int depth);
    void distributeKeys(BTreePage* leftSibling, BTreePage* rightSibling, BTreePage* parent, BTreeRecord** recordToAdd, int depth, bool changeParentNodes);
    void distributeChildren(BTreePage* leftSibling, BTreePage* rightSibling);
    int getRootId();
    BTreePage* createNewRoot();
};



#endif //BTREE_H
