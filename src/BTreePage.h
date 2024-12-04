#ifndef BTREERECORD_H
#define BTREERECORD_H
#include <vector>
#include "BTreeRecord.h"
using namespace std;

class BTreeRecord;

class BTreePage {
private:
    unsigned int size;
    unsigned int* parentPageAddress;
    vector<BTreeRecord*>* records;
    vector<BTreePage*>* children;  // po napisaniu b drzewa będzie trzeba zmienić na numer bajtu w pliku
public:
    BTreePage();
    ~BTreePage();
    void deleteChildren();
    void loadBTreePage(unsigned int pageAddress);
};



#endif //BTREERECORD_H
