#include <iostream>
#include <ostream>
#include "Program.h"
using namespace std;

Program::Program(): bTree(&this->dataManager) {
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
            // TODO: PRZETESTUJ SZUKANIE
            case 1: {
                // TODO: DODAĆ GENEROWANIE REKORDÓW
                FileRecord* record = this->getRecordInput();
                int pageNumber = this->dataManager.getNextFreePageNumber();
                if (this->bTree.insertRecord(pageNumber, record->getKey())) {
                    cout << "Wprowadzono nowy rekord" << endl;
                    FileRecord* newRecord = new FileRecord();
                    this->dataManager.insertRecordToDiskPage(newRecord, pageNumber);
                    this->dataManager.increaseDataPageRecordsCount();
                }
                else {
                    cout << "Rekord z takim kluczem juz znajduje sie w pliku" << endl;
                }
                break;
            }
            case 2: {
                int key;
                cout << "Podaj klucz: " << endl;
                cin >> key;
                FileRecord* foundRecord = this->bTree.searchRecord(key);
                if (foundRecord != nullptr) {
                    cout << "Znaleziono rekord z kluczem " << key << endl;
                    cout << foundRecord->toString() << endl;
                }
                else {
                    cout << "Rekord o podanym kluczu nie zostal odnaleziony" << endl;
                }
                break;
            }
            case 3: {
                // wyświetlenie drzewa - najlepiej chyba będzie je wyświetlać jakoś poziomami
                break;
            }
            case 4: {
                // tutaj powinno nastąpić odczytanie danych z pliku wejściowego i utworzenie nowego b-drzewa
                // operacje typu wstaw, aktualizuj, (usuń?) - zależy czy zdążysz
                break;
            }
            case 5: {
                // odczyt instrukcji z pliku wejściowego
                break;
            }
            case 6: {
                return;
            }
            default: {
                cout << "Wprowadzono nieprawidlowa opcje" << endl;
                break;
            }
        }
    }
}

FileRecord *Program::getRecordInput() {
    int key;
    double a, b, h;
    cout << "Podaj klucz dla nowego elementu: " << endl;
    cin >> key;
    cout << "Podaj a: " << endl;
    cin >> a;
    cout << "Podaj b: " << endl;
    cin >> b;
    cout << "Podaj h: " << endl;
    cin >> h;
    return new FileRecord(a, b, h, key);
}

