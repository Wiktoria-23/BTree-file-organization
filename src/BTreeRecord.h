#ifndef BTREERECORD_H
#define BTREERECORD_H



class BTreeRecord {
private:
    unsigned int key;
    unsigned int addressInFile;
public:
    BTreeRecord();
    BTreeRecord(unsigned int key, unsigned int addressInFile);
    ~BTreeRecord();
};



#endif //BTREERECORD_H
