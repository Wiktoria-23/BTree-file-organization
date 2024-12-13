#ifndef PROGRAM_H
#define PROGRAM_H
#include <fstream>
#include <string>
#include "BTree.h"
#include "DataManager.h"
using namespace std;

#define BTREE_ORDER 2
#define BTREE_PAGE_SIZE (2 * BTREE_ORDER * 3 * sizeof(int) + (4 * sizeof(int)))
#define DATA_RECORD_SIZE (sizeof(recordData))
#define DATA_PAGE_SIZE (DATA_RECORD_SIZE * 4)

class Program {
private:
    BTree bTree;
    DataManager dataManager;
public:
    Program();
    ~Program();
    void run();
    FileRecord* getRecordInput();
    void printTree();
    void printData();
};



#endif //PROGRAM_H
