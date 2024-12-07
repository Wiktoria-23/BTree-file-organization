#include "FileRecord.h"

FileRecord::FileRecord() {
    this->a = 0;
    this->b = 0;
    this->h = 0;
}

FileRecord::FileRecord(double a, double b, double h, int key) {
    this->a = a;
    this->b = b;
    this->h = h;
    this->key = key;
}

FileRecord::~FileRecord() {

}

double FileRecord::calculateField() {
    return (this->a + this->b) * this->h / 2;
}

double FileRecord::getA() {
    return this->a;
}

double FileRecord::getB() {
    return this->b;
}

double FileRecord::getH() {
    return this->h;
}

int FileRecord::getKey() {
    return this->key;
}

string FileRecord::toString() {
    return ("Klucz: " + to_string(this->key) + ", a: " + to_string(this->a) + ", b: " + to_string(this->b) + ", h: " + to_string(this->h));
}