#include <cassert>
#include <iostream>
#include <cstdio>
#include "../InsuraProCRM/FileManager.h"
#include "../InsuraProCRM/Rubrica.h"

int main() {
    // caricamento file valido
    Rubrica r1;
    FileManager::caricaClienti(r1, "tests/fixtures/clienti_ok.csv");
    assert(r1.visualizzaTutti().size() == 3);
    Cliente c(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(r1.getById(2, c) == true);
    assert(c.getNome() == "Anna");
    assert(r1.getProssimoId() == 4); // max id (3) + 1

    // caricamento file con riga malformata: scartata, le altre caricate
    Rubrica r2;
    FileManager::caricaClienti(r2, "tests/fixtures/clienti_malformato.csv");
    assert(r2.visualizzaTutti().size() == 2); // riga 2 scartata

    // file inesistente: nessun errore, rubrica vuota
    Rubrica r3;
    FileManager::caricaClienti(r3, "tests/fixtures/clienti_assente_non_esiste.csv");
    assert(r3.visualizzaTutti().size() == 0);

    // round-trip: salvo e ricarico, deve dare lo stesso contenuto logico
    Rubrica sorgente;
    sorgente.aggiungiCliente("Giulia", "Neri", "3331112223", "giulia.neri@email.com",
                              "NRIGLI88E44H501U", "12/06/1988");
    sorgente.aggiungiCliente("Paolo", "Blu", "3334445556", "paolo.blu@email.com",
                              "BLUPLA79F10H501U", "20/07/1979");
    FileManager::salvaClienti(sorgente, "tests/fixtures/clienti_roundtrip.csv");

    Rubrica ricaricata;
    FileManager::caricaClienti(ricaricata, "tests/fixtures/clienti_roundtrip.csv");
    assert(ricaricata.visualizzaTutti().size() == 2);
    Cliente verifica(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(ricaricata.getById(1, verifica) == true);
    assert(verifica.getNome() == "Giulia");
    assert(verifica.getEmail() == "giulia.neri@email.com");

    std::remove("tests/fixtures/clienti_roundtrip.csv");

    std::cout << "test_filemanager_clienti: tutti i test passati" << std::endl;
    return 0;
}
