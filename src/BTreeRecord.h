#ifndef BTREERECORD_H
#define BTREERECORD_H



class BTreeRecord {
private:
    int key;
    int dataPageNumber;
public:
    BTreeRecord();
    BTreeRecord(int newKey, int newDataPageNumber);
    ~BTreeRecord();
    int getKey();
    void setKey(int newKey);
    int getPageNumberInFile();
    void setDataPageNumberInFile(int newDataPageNumber);
};



#endif //BTREERECORD_H
