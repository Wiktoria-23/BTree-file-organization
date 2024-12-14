#include <iostream>
#include <ostream>
#include "Program.h"
using namespace std;

Program::Program(): bTree(&this->dataManager) {
    random_device random_device;
    numbersGeneratorDouble = new default_random_engine(random_device());
    numbersDistributionDouble = new uniform_real_distribution<double>(0, 100);
    numbersGeneratorInt = new default_random_engine(random_device());
    numbersDistributionInt = new uniform_int_distribution<int>(0, 100);
}

Program::~Program() {

}


// TODO: DODAĆ ZLICZANIE I WYŚWIETLANIE LICZBY OPERACJI DYSKOWYCH PO KAŻDEJ OPERACJI WYKONYWANEJ W PROGRAMIE
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
                cout << "Czy chcesz wygenerowac nowy rekord? (1 - tak, 0 - nie)" << endl;
                int option1;
                cin >> option1;
                FileRecord* record;
                if (option1 == 1) {
                    record = new FileRecord(numbersGeneratorDouble, numbersDistributionDouble, numbersGeneratorInt, numbersDistributionInt);
                }
                else {
                    record = getRecordInput();
                }
                int pageNumber = dataManager.getlastDataPageNumber();
                if (bTree.insertRecord(pageNumber, record->getKey())) {
                    cout << "Wprowadzono nowy rekord" << endl;
                    dataManager.insertRecordToDiskPage(record, pageNumber);
                    dataManager.increaseDataPageRecordsCount();
                }
                else {
                    cout << "Rekord z takim kluczem juz znajduje sie w pliku" << endl;
                }
                delete record;
                cout << "Czy wyswietlic zawartosc indeksu oraz pliku z danymi (1 - tak, 0 - nie)" << endl;
                int option2;
                cin >> option2;
                if (option2 == 1) {
                    printTree();
                    printData();
                }
                break;
            }
            case 2: {
                int key;
                cout << "Podaj klucz: " << endl;
                cin >> key;
                int recordPage = bTree.searchRecord(key);
                if (recordPage != 0) {
                    cout << "Znaleziono rekord z kluczem " << key << endl;
                    // odczytaj rekord ze strony
                    FileRecord* foundRecord = dataManager.readRecordFromDiskPage(key, recordPage);
                    cout << foundRecord->toString() << endl;
                    delete foundRecord;
                }
                else {
                    cout << "Rekord o podanym kluczu nie zostal odnaleziony" << endl;
                }
                break;
            }
            case 3: {
                int key;
                cout << "Podaj klucz: " << endl;
                cin >> key;
                int recordPage = bTree.searchRecord(key);
                if (recordPage != 0) {
                    FileRecord* updatedRecord = getRecordInput();
                    dataManager.updateRecordOnDiskPage(updatedRecord, recordPage);
                }
                else {
                    cout << "Rekord o podanym kluczu nie istnieje" << endl;
                }
                break;
            }
            case 4: {
                printTree();
                printData();
                break;
            }
            case 5: {
                // TODO: DODAJ ODCZYTYWANIE INSTRUKCJI Z PLIKU WEJŚCIOWEGO
                // tutaj powinno nastąpić odczytanie danych z pliku wejściowego i utworzenie nowego b-drzewa
                // operacje typu wstaw, aktualizuj, (usuń?) - zależy czy zdążysz
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

void Program::printTree() {
    cout << "Zawartosc indeksu (b-drzewa)" << endl;
    vector<int> pagesToVisit;
    pagesToVisit.push_back(bTree.getRootId());
    while (!pagesToVisit.empty()) {
        // TODO: TUTAJ TEŻ ZMIEŃ ŻEBY NIEPOTRZEBNIE NIE CZYTAĆ STRON, KTÓRE MASZ JUŻ W BUFORZE
        BTreePage* currentPage = dataManager.loadBTreePage(pagesToVisit.front());
        if (currentPage != nullptr) {
            cout << currentPage->toString() << endl;
            pagesToVisit.insert(pagesToVisit.end(), currentPage->getChildrenIds()->begin(), currentPage->getChildrenIds()->end());
            delete currentPage;
        }
        pagesToVisit.erase(pagesToVisit.begin());
    }
    cout << "Koniec zawartosci indeksu" << endl << endl;
}

void Program::printData() {
    // wypisanie pliku z danymi to wypisanie danych w pliku po kolei
    cout << "Dane zapisane w pliku" << endl;
    for (int i = 1; i <= dataManager.getlastDataPageNumber(); i++) {  // pobieramy numer strony, która ma wolne miejsce (coś może na niej być lub nie)
        vector<FileRecord>* records = dataManager.readRecordsDiskPage(i);
        cout << "Strona nr " << i << ". " << endl;
        for (int i = 0; i < records->size(); i++) {
            cout << records->at(i).toString() << endl;
        }
    }
    cout << "Koniec zawartosci pliku z danymi" << endl << endl;
}
