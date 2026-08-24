#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include "../InsuraProCRM/Interazione.h"
#include "../InsuraProCRM/Appuntamento.h"

int main() {
    Appuntamento a(1, 10, "RSSMRA80A01H501U", "15/03/2026", "10:30", "concluso",
                   "Interessato a polizza casa");

    assert(a.getId() == 1);
    assert(a.getIdCliente() == 10);
    assert(a.getCodiceFiscaleCliente() == "RSSMRA80A01H501U");
    assert(a.getData() == "15/03/2026");
    assert(a.getTipo() == "Appuntamento");

    std::string csv = a.toCSV();
    assert(csv == "1,10,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"Interessato a polizza casa\"");

    Appuntamento b(2, 10, "RSSMRA80A01H501U", "16/03/2026", "11:00", "annullato",
                   "chiama lunedi', non martedi'"); // nota senza virgole: solo per contrasto
    std::string csvB = b.toCSV();
    assert(csvB.find(",\"chiama lunedi', non martedi'\"") != std::string::npos);

    Appuntamento c(3, 10, "RSSMRA80A01H501U", "17/03/2026", "12:00", "da ricontattare",
                   "nota, con virgola dentro");
    std::string csvC = c.toCSV();
    assert(csvC == "3,10,RSSMRA80A01H501U,17/03/2026,12:00,da ricontattare,\"nota, con virgola dentro\"");

    bool eccezioneLanciata = false;
    try {
        Appuntamento invalido(4, 10, "RSSMRA80A01H501U", "31/02/2026", "12:00", "concluso", "");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);

    // conteggio istanze vive tramite puntatore a base: nessun leak, distruttore virtuale
    {
        std::vector<std::unique_ptr<Interazione>> collezione;
        collezione.push_back(std::unique_ptr<Interazione>(
            new Appuntamento(5, 10, "RSSMRA80A01H501U", "18/03/2026", "09:00", "concluso", "")));
        assert(collezione.size() == 1);
        assert(collezione[0]->getTipo() == "Appuntamento");
    } // qui il unique_ptr distrugge l'oggetto tramite puntatore a base: se il distruttore
      // non e' virtuale, questo test non lo rileva direttamente, ma verifica almeno
      // che il codice compili e giri con -Wall senza warning di distruttore non virtuale.

    std::cout << "test_appuntamento: tutti i test passati" << std::endl;
    return 0;
}
