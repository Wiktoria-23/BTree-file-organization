#ifndef PROGRAM_H
#define PROGRAM_H
#include <fstream>
#include <string>
#include "BTree.h"
using namespace std;


class Program {
private:
    BTree* bTree;
    fstream bTreeFileStream;
    string bTreeFilename;
    fstream dataFileStream;
    string dataFilename;
public:
    Program();
    ~Program();
    void run();
    void ensureFileExist(string filename, fstream& fileStream);
};



#endif //PROGRAM_H
