#include <cassert>
#include <iostream>
#include "../InsuraProCRM/FileManager.h"

int main() {
    auto campi1 = FileManager::parseRigaCSV(
        "1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980");
    assert(campi1.size() == 7);
    assert(campi1[1] == "Mario");
    assert(campi1[6] == "01/01/1980");

    auto campi2 = FileManager::parseRigaCSV(
        "1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"Interessato a polizza casa\"");
    assert(campi2.size() == 7);
    assert(campi2[6] == "Interessato a polizza casa"); // senza virgolette

    auto campi3 = FileManager::parseRigaCSV(
        "1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"nota, con virgola dentro\"");
    assert(campi3.size() == 7); // la virgola interna NON deve spezzare il campo
    assert(campi3[6] == "nota, con virgola dentro");

    auto campiVuoti = FileManager::parseRigaCSV("");
    assert(campiVuoti.size() == 0);

    auto campiMalformati = FileManager::parseRigaCSV("1,Mario,Rossi");
    assert(campiMalformati.size() == 3); // il parser non giudica, restituisce cio' che trova

    std::cout << "test_csv_parsing: tutti i test passati" << std::endl;
    return 0;
}
