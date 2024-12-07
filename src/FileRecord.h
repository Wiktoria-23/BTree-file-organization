#ifndef FILERECORD_H
#define FILERECORD_H
#include <string>
using namespace std;

class FileRecord {
private:
    int key;
    double a;
    double b;
    double h;
public:
    FileRecord();
    ~FileRecord();
    double calculateField();
    double getA();
    double getB();
    double getH();
    int getKey();
    FileRecord(double a, double b, double h, int key);
    string toString();
};



#endif //FILERECORD_H
