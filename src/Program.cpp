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
    delete numbersGeneratorDouble;
    delete numbersDistributionDouble;
    delete numbersGeneratorInt;
    delete numbersDistributionInt;
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
                handleRecordAdding();
                printDiskAccessInfo();
                dataManager.resetDiskAccesses();
                break;
            }
            case 2: {
                int key;
                cout << "Podaj klucz: " << endl;
                cin >> key;
                searchForRecord(key);
                printDiskAccessInfo();
                dataManager.resetDiskAccesses();
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
                printDiskAccessInfo();
                dataManager.resetDiskAccesses();
                break;
            }
            case 4: {
                printTree();
                printData();
                printDiskAccessInfo();
                dataManager.resetDiskAccesses();
                break;
            }
            case 5: {
                readInstructionsFromFile();
                printDiskAccessInfo();
                dataManager.resetDiskAccesses();
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
        BTreePage* currentPage = dataManager.loadBTreePage(pagesToVisit.front(), false);
        if (currentPage != nullptr) {
            bool found = false;
            cout << currentPage->toString() << endl;
            pagesToVisit.insert(pagesToVisit.end(), currentPage->getChildrenIds()->begin(), currentPage->getChildrenIds()->end());
            for (int i = 0; i < dataManager.getVisitedPages()->size(); i++) {
                 if (dataManager.getVisitedPages()->at(i)->getPageId() == currentPage->getPageId()) {
                     found = true;
                     break;
                 }
            }
            if (!found) {
                delete currentPage;     // zapobieganie usuwaniu węzła z bufora
            }
        }
        pagesToVisit.erase(pagesToVisit.begin());
    }
    cout << "Koniec zawartosci indeksu" << endl << endl;
}

void Program::printData() {
    // wypisanie pliku z danymi to wypisanie danych w pliku po kolei
    cout << "Dane zapisane w pliku" << endl;
    for (int i = 1; i <= dataManager.getlastDataPageNumber(); i++) {  // pobieramy numer strony, która ma wolne miejsce (coś może na niej być lub nie)
        vector<FileRecord>* records = dataManager.readRecordsDiskPage(i, false);
        cout << "Strona nr " << i << ". " << endl;
        for (int i = 0; i < records->size(); i++) {
            cout << records->at(i).toString() << endl;
        }
    }
    cout << "Koniec zawartosci pliku z danymi" << endl << endl;
}

void Program::readInstructionsFromFile() {
    fstream instructionsStream;
    instructionsStream.open("../instructions.txt", ios::in);
    if (!instructionsStream) {
        cout << "Plik z instrukcjami nie istnieje!" << endl;
        return;
    }
    dataManager.resetFiles();
    int option, key;
    double a, b, h;
    while (instructionsStream >> option) {
        instructionsStream >> key;
        if (option == 1) {
            instructionsStream >> a >> b >> h;
            FileRecord* record = new FileRecord(a, b, h, key);
            addNewRecordToBTree(record);
            delete record;
        }
        else {
            searchForRecord(key);
        }
    }
    cout << "Odczytywanie instrukcji z pliku zakonczone" << endl;
    instructionsStream.close();
}

void Program::addNewRecordToBTree(FileRecord* record) {
    int pageNumber = dataManager.getlastDataPageNumber();
    if (bTree.insertRecord(pageNumber, record->getKey())) {
        cout << "Wprowadzono nowy rekord" << endl;
        dataManager.insertRecordToDiskPage(record, pageNumber);
        dataManager.increaseDataPageRecordsCount();
    }
    else {
        cout << "Rekord z takim kluczem juz znajduje sie w pliku" << endl;
    }
}

void Program::searchForRecord(int key) {
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
}

void Program::handleRecordAdding() {
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
    addNewRecordToBTree(record);
    delete record;
    cout << "Czy wyswietlic zawartosc indeksu oraz pliku z danymi (1 - tak, 0 - nie)" << endl;
    int option2;
    cin >> option2;
    if (option2 == 1) {
        printTree();
        printData();
    }
}

void Program::printDiskAccessInfo() {
    cout << "Podczas operacji zostalo wykonanych " + to_string(dataManager.getDiskPageReadsNumber()) + " odczytow dysku ";
    cout << "oraz " + to_string(dataManager.getDiskPageWritesNumber()) + " zapisow dysku" << endl;
}
