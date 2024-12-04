#include "BTree.h"

BTree::BTree() {
    rootAddress = nullptr;
}

BTree::~BTree() {
    currentPage->loadBTreePage(*rootAddress);
    currentPage->deleteChildren();
    delete rootAddress;
}

bool BTree::insertRecord() {
    unsigned int key;
    double a, b, h;
    cout << "Podaj klucz dla nowego elementu: " << endl;
    cin >> key;
    cout << "Podaj a: " << endl;
    cin >> a;
    cout << "Podaj b: " << endl;
    cin >> b;
    cout << "Podaj h: " << endl;
    cin >> h;
    if (rootAddress == nullptr) {
        // dodanie pierwszej strony b-drzewa z jednym elementem
    }
    // tutaj powinno odbyć się szukanie elementu z podanym kluczem - jeżeli go nie znajdziemy to możemy go dodać w znalezione miejsce
    return true;
}

FileRecord *BTree::searchRecord(unsigned int key) {
    if (rootAddress == nullptr) {
        return nullptr;
    }
    return nullptr;
}

void BTree::updateRecord(unsigned int key) {
    // to będzie wyszukanie rekordu - z użyciem funkcji search, a następnie nadpisanie całej strony dyskowej
}
