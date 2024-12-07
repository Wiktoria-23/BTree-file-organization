#ifndef BTREEPAGE_H
#define BTREEPAGE_H
#include <vector>
#include "BTreeRecord.h"
using namespace std;

class BTreeRecord;

class BTreePage {
private:
    // WAŻNE - STRONY NA DYSKU NUMERUJEMY OD 1, A 0 JEST ZAREZERWOWANE (WSKAZYWANIE ŻE WĘZEŁ NIE MA RODZICA)
    int pageId;
    int keysNumber;
    int childrenNumber;
    int parentPageId;
    vector<BTreeRecord>* records;
    vector<int>* childrensPagesIds;
public:
    BTreePage();
    ~BTreePage();
    void deleteChildren();
    void setKeysNumber(int keysNumber);
    int getKeysNumber();
    void setRecords(vector<BTreeRecord>* records);
    vector<BTreeRecord>* getRecords();
    void setParentId(int parentPageId);
    int getParentId();
    void setChildrenIds(vector<int>* childrensPagesIds);
    vector<int>* getChildrenIds();
    void setPageId(int pageId);
    void addNewRecord(int key, int dataPageNumber);
    void addNewChildId(int childId);
    int getPageId();
    int searchKey(int key);
    int getChildrenNumber();
    void setChildrenNumber(int childrenNumber);
};



#endif //BTREEPAGE_H
