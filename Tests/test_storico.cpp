#include <cassert>
#include <iostream>
#include "../InsuraProCRM/StoricoInterazioni.h"

int main() {
    StoricoInterazioni storico;

    int idApp = storico.aggiungiAppuntamento(1, "RSSMRA80A01H501U", "15/03/2026",
                                              "10:30", "concluso", "");
    int idContr = storico.aggiungiContratto(1, "RSSMRA80A01H501U", "16/03/2026",
                                             "Auto", 450.0, "16/03/2027");
    int idApp2 = storico.aggiungiAppuntamento(2, "BNCLCU90C03H501U", "17/03/2026",
                                               "09:00", "concluso", "");

    assert(idApp == 1);
    assert(idContr == 2); // contatore condiviso, cresce indipendentemente dal tipo
    assert(idApp2 == 3);

    auto perCliente1 = storico.getPerCliente(1, "");
    assert(perCliente1.size() == 2);

    auto perCliente2 = storico.getPerCliente(2, "");
    assert(perCliente2.size() == 1);

    auto perClienteInesistente = storico.getPerCliente(99, "");
    assert(perClienteInesistente.size() == 0);

    assert(storico.eliminaInterazione(idApp) == true);
    assert(storico.getPerCliente(1, "").size() == 1);
    assert(storico.eliminaInterazione(999) == false); // non trovato, nessun crash

    std::cout << "test_storico: tutti i test passati" << std::endl;
    return 0;
}
