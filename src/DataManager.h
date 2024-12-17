#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include <fstream>
#include "FileRecord.h"
using namespace std;

// struct for saving and reading records in file with data
typedef struct recordData {
    int key;
    double a;
    double b;
    double h;
} recordData;

class DataManager {
private:
    vector<FileRecord> recordsDiskPage;
    fstream bTreeFileStream;
    string bTreeFilename;
    fstream dataFileStream;
    string dataFilename;
    int lastDataPageNumber;  // last page that has free space to save new data
    int dataPageRecordsCount;
    int nextFreeBTreePageNumber;
    vector<BTreePage*>* visitedPages; // buffer to save visited pages data (buffer size is tree height)
    int diskPageReads;
    int diskPageWrites;
public:
    DataManager();
    ~DataManager();
    void resetFile(string filename, fstream &fileStream);
    void resetFiles();
    BTreePage* loadBTreePage(int pageNumber, bool countDiskAccess);
    void saveBTreePage(BTreePage* page, bool deleting, bool countDiskAccess);
    int getNextFreePageNumber();
    void increasePageNumber();
    vector<FileRecord>* readRecordsDiskPage(int diskPageNumber, bool countDiskAccess);
    void saveRecordsDiskPage(vector<FileRecord>* recordsDiskPage, int pageNumber, bool countDiskAccess);
    int getlastDataPageNumber();
    void increaseDataPageRecordsCount();
    void insertRecordToDiskPage(FileRecord* record, int pageNumber);
    FileRecord* readRecordFromDiskPage(int key, int diskPageNumber, bool countDiskAccess);
    void updateRecordOnDiskPage(FileRecord* record, int pageNumber);
    vector<BTreePage*>* getVisitedPages();
    void resetDiskAccesses();
    int getDiskPageReadsNumber();
    int getDiskPageWritesNumber();
};



#endif //DATAMANAGER_H
