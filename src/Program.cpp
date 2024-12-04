#include <iostream>
#include <ostream>
#include "Program.h"
using namespace std;

Program::Program() {
    this->dataFilename = "../data.dat";
    this->bTreeFilename = "../bTreeData.dat";
    this->ensureFileExist(this->bTreeFilename, this->bTreeFileStream);
    this->ensureFileExist(this->dataFilename, this->dataFileStream);
}

Program::~Program() {

}

void Program::run() {
    while (true) {
        cout << "Wprowadz nastepna instrukcje do wykonania:" << endl;
        cout << "1. Wprowadz nowy rekord" << endl;
        cout << "2. Szukaj rekordu" << endl;
        cout << "3. Aktualizuj rekord" << endl;
        cout << "4. Wyswietl plik" << endl;
        cout << "5. Wczytaj dane wejsciowe" << endl;
        cout << "6. Wyjdz" << endl;
        int option;
        cin >> option;
        switch (option) {
            case 1: {
                if (this->bTree->insertRecord()) {
                    cout << "Wprowadzono nowy rekord" << endl;
                }
                else {
                    cout << "Rekord z takim kluczem juz znajduje sie w pliku" << endl;
                }
            }
            case 2: {
                unsigned int key;
                cout << "Podaj klucz: " << endl;
                cin >> key;
                FileRecord* foundRecord = this->bTree->searchRecord(key);
                if (foundRecord != nullptr) {
                    cout << "Znaleziono rekord z kluczem " << key << endl;
                    //cout << foundRecord->toString() << endl;
                }
            }
            case 3: {
                // wyświetlenie drzewa - najlepiej chyba będzie je wyświetlać jakoś poziomami
            }
            case 4: {
                // tutaj powinno nastąpić odczytanie danych z pliku wejściowego i utworzenie nowego b-drzewa
                // operacje typu wstaw, aktualizuj, (usuń?) - zależy czy zdążysz
            }
            case 5: {
                // odczyt instrukcji z pliku wejściowego
            }
            case 6: {
                return;
            }
            default: {
                cout << "Wprowadzono nieprawidlowa opcje" << endl;
            }
        }
    }
}

void Program::ensureFileExist(string filename, fstream &fileStream) {
    fileStream.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!fileStream) {
        fileStream.clear();
        fileStream.open(filename, std::ios::binary | std::ios::out);
    }
    fileStream.close();
}
