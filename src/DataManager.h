#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include <fstream>
#include "FileRecord.h"
using namespace std;

// struktura do zapisu i odczytu rekordów z pliku z danymi
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
    int lastDataPageNumber;  // przechowujemy adres ostatniej strony, w której znajduje się miejsce, by móc ją łatwo odczytać
    int dataPageRecordsCount;
    int nextFreeBTreePageNumber;
    vector<BTreePage*>* visitedPages; // bufor do zapisywania odwiedzonych stron (wysokość bufora to wysokość drzewa)
public:
    DataManager();
    ~DataManager();
    void resetFile(string filename, fstream &fileStream);
    void resetFiles();
    BTreePage* loadBTreePage(int pageNumber);
    void saveBTreePage(BTreePage* page, bool deleting);
    int getNextFreePageNumber();
    void increasePageNumber();
    vector<FileRecord>* readRecordsDiskPage(int diskPageNumber);
    void saveRecordsDiskPage(vector<FileRecord>* recordsDiskPage, int pageNumber);
    int getlastDataPageNumber();
    void increaseDataPageRecordsCount();
    void insertRecordToDiskPage(FileRecord* record, int pageNumber);
    FileRecord* readRecordFromDiskPage(int key, int diskPageNumber);
    void updateRecordOnDiskPage(FileRecord* record, int pageNumber);
    vector<BTreePage*>* getVisitedPages();
};



#endif //DATAMANAGER_H
