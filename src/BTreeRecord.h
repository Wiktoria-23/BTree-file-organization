#ifndef BTREERECORD_H
#define BTREERECORD_H



class BTreeRecord {
private:
    int key;
    int dataPageNumber;
public:
    BTreeRecord();
    BTreeRecord(int key, int dataPageNumber);
    ~BTreeRecord();
    int getKey();
    void setKey(int key);
    int getPageNumberInFile();
    void setDataPageNumberInFile(int dataPageNumber);
};



#endif //BTREERECORD_H
