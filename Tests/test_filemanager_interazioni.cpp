#include <cassert>
#include <iostream>
#include <cstdio>
#include "../InsuraProCRM/FileManager.h"
#include "../InsuraProCRM/StoricoInterazioni.h"

int main() {
    // caricamento: contatore condiviso = max(5, 3) + 1 = 6
    StoricoInterazioni s1;
    FileManager::caricaInterazioni(s1, "tests/fixtures/appuntamenti_ok.csv", "tests/fixtures/contratti_ok.csv");
    assert(s1.tutte().size() == 4);
    assert(s1.getProssimoId() == 6);

    auto perCliente1 = s1.getPerCliente(1, "");
    assert(perCliente1.size() == 2); // 1 appuntamento + 1 contratto

    // riga malformata in contratti: scartata, le altre caricate
    StoricoInterazioni s2;
    FileManager::caricaInterazioni(s2, "tests/fixtures/appuntamenti_ok.csv",
                          "tests/fixtures/contratti_malformato.csv");
    // 2 appuntamenti validi + 2 contratti validi (id 2 e 4), 1 contratto scartato (id 3)
    assert(s2.tutte().size() == 4);

    // round-trip
    StoricoInterazioni sorgente;
    sorgente.aggiungiAppuntamento(9, "AAABBB11C22D333E", "01/01/2026", "08:00", "concluso", "");
    sorgente.aggiungiContratto(9, "AAABBB11C22D333E", "02/01/2026", "Vita", 200.0, "02/01/2027");
    FileManager::salvaInterazioni(sorgente, "tests/fixtures/appuntamenti_roundtrip.csv",
                         "tests/fixtures/contratti_roundtrip.csv");

    StoricoInterazioni ricaricato;
    FileManager::caricaInterazioni(ricaricato, "tests/fixtures/appuntamenti_roundtrip.csv",
                          "tests/fixtures/contratti_roundtrip.csv");
    assert(ricaricato.tutte().size() == 2);
    auto perCliente9 = ricaricato.getPerCliente(9, "");
    assert(perCliente9.size() == 2);
    // un Contratto non deve finire nel file appuntamenti e viceversa: verificato
    // dal fatto che entrambi i file, ricaricati separatamente, tornano il tipo giusto
    bool trovatoAppuntamento = false, trovatoContratto = false;
    for (const auto* i : perCliente9) {
        if (i->getTipo() == "Appuntamento") trovatoAppuntamento = true;
        if (i->getTipo() == "Contratto") trovatoContratto = true;
    }
    assert(trovatoAppuntamento && trovatoContratto);

    std::remove("tests/fixtures/appuntamenti_roundtrip.csv");
    std::remove("tests/fixtures/contratti_roundtrip.csv");

    std::cout << "test_filemanager_interazioni: tutti i test passati" << std::endl;
    return 0;
}
