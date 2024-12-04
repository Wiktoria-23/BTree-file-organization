#ifndef FILERECORD_H
#define FILERECORD_H
#include <string>
using namespace std;

class FileRecord {
private:
    unsigned int key;
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
    unsigned int getKey();
    FileRecord(double a, double b, double h, unsigned int key);
    string toString();
};



#endif //FILERECORD_H
