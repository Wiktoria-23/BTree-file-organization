#ifndef PROGRAM_H
#define PROGRAM_H
#include <fstream>
#include <random>
#include <string>
#include "BTree.h"
#include "DataManager.h"
using namespace std;

#define BTREE_ORDER 2
#define BTREE_PAGE_SIZE (2 * BTREE_ORDER * 3 * sizeof(int) + (5 * sizeof(int)))
#define DATA_RECORD_SIZE (sizeof(recordData))
#define DATA_PAGE_SIZE (DATA_RECORD_SIZE * 4)

class Program {
private:
    BTree bTree;
    DataManager dataManager;
    default_random_engine* numbersGeneratorDouble;
    uniform_real_distribution<double>* numbersDistributionDouble;
    default_random_engine* numbersGeneratorInt;
    uniform_int_distribution<int>* numbersDistributionInt;
public:
    Program();
    ~Program();
    void run();
    FileRecord* getRecordInput();
    void printTree();
    void printData();
    void readInstructionsFromFile();
    void addNewRecordToBTree(FileRecord* record);
    void searchForRecord(int key);
    void handleRecordAdding();
    void printDiskAccessInfo();
};



#endif //PROGRAM_H
