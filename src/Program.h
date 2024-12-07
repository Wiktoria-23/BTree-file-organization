#ifndef PROGRAM_H
#define PROGRAM_H
#include <fstream>
#include <string>
#include "BTree.h"
#include "DataManager.h"
using namespace std;

#define BTREE_ORDER 5
#define BTREE_PAGE_SIZE (2 * BTREE_ORDER * 3 * sizeof(int) + (5 * sizeof(int)))
#define DATA_RECORD_SIZE (3 * sizeof(double) + sizeof(int))
#define DATA_PAGE_SIZE (DATA_RECORD_SIZE * 10)

class Program {
private:
    BTree bTree;
    DataManager dataManager;
public:
    Program();
    ~Program();
    void run();
    FileRecord* getRecordInput();
};



#endif //PROGRAM_H
