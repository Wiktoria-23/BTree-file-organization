#ifndef FILERECORD_H
#define FILERECORD_H
#include <random>
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
    FileRecord(double newA, double newB, double newH, int newKey);
    FileRecord(
        default_random_engine *generatorDouble, uniform_real_distribution<double> *distributionDouble,
        default_random_engine *generatorInt, uniform_int_distribution<int> *distributionInt
        );
    string toString();
};



#endif //FILERECORD_H
