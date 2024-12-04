#include "BTreePage.h"

BTreePage::BTreePage() {
    this->parentPageAddress = nullptr;
    this->records = new vector<BTreeRecord*>;
    this->children = new vector<BTreePage*>;
}

BTreePage::~BTreePage() {
    // chyba będzie trzeba jeszcze wyczyścić pamięć wektorów
    delete this->records;
    this->deleteChildren();
}

void BTreePage::deleteChildren() {
    // tutaj to powinno być zmienione na wersję z ładowaniem stron
    /*// rekurencyjne usuwanie dzieci węzła
    if (this->children->size() > 0) {
        for (BTreePage* child : this->children) {
            child->deleteChildren();
            delete child;
        }
    }
    delete this->children;*/
}

void BTreePage::loadBTreePage(unsigned int pageAddress) {
    // otwarcie pliku
    // przejście do odpowiedniego bajtu pliku (pageAdress - numer bajtu w pliku)
    // załadowanie danych do tablicy
}
