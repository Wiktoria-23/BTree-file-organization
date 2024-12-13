#include "FileRecord.h"

FileRecord::FileRecord() {
    a = 0;
    b = 0;
    h = 0;
}

FileRecord::FileRecord(double newA, double newB, double newH, int newKey) {
    a = newA;
    b = newB;
    h = newH;
    key = newKey;
}

FileRecord::~FileRecord() {

}

double FileRecord::calculateField() {
    return (a + b) * h / 2;
}

double FileRecord::getA() {
    return a;
}

double FileRecord::getB() {
    return b;
}

double FileRecord::getH() {
    return h;
}

int FileRecord::getKey() {
    return key;
}

string FileRecord::toString() {
    return ("Klucz: " + to_string(key) + ", a: " + to_string(a) + ", b: " + to_string(b) + ", h: " + to_string(h));
}

FileRecord::FileRecord(default_random_engine* generatorDouble, uniform_real_distribution<double>* distributionDouble,
    default_random_engine* generatorInt, uniform_int_distribution<int>* distributionInt) {
    a = (*distributionDouble)(*generatorDouble);
    b = (*distributionDouble)(*generatorDouble);
    h = (*distributionDouble)(*generatorDouble);
    key = (*distributionInt)(*generatorInt);
}
