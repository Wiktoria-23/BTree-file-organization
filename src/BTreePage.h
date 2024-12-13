#ifndef BTREEPAGE_H
#define BTREEPAGE_H
#include <vector>
#include <string>
#include "BTreeRecord.h"
using namespace std;

class BTreeRecord;

class BTreePage {
private:
    // WAŻNE - STRONY NA DYSKU NUMERUJEMY OD 1, A 0 JEST ZAREZERWOWANE (WSKAZYWANIE ŻE WĘZEŁ NIE MA RODZICA)
    int pageId;
    int parentPageId;
    vector<BTreeRecord*>* records;
    vector<int>* childrensPagesIds;
public:
    BTreePage();
    ~BTreePage();
    void deleteChildren();
    void setRecords(vector<BTreeRecord*>* newRecords);
    vector<BTreeRecord*>* getRecords();
    void setParentId(int newParentPageId);
    int getParentId();
    void setChildrenIds(vector<int>* newChildrensPagesIds);
    vector<int>* getChildrenIds();
    void setPageId(int newPageId);
    void addNewRecord(BTreeRecord* recordToAdd);
    void addNewChildId(int childId, int index);
    int getPageId();
    int searchKey(int key);
    int findInsertIndex(int key);
    int findChildIndex(int childId);
    string toString();
};



#endif //BTREEPAGE_H
