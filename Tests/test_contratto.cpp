#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../InsuraProCRM/Contratto.h"

int main() {
    Contratto c(1, 10, "RSSMRA80A01H501U", "15/03/2026", "Auto", 450.00, "15/03/2027");

    assert(c.getId() == 1);
    assert(c.getTipo() == "Contratto");
    assert(c.toCSV() == "1,10,RSSMRA80A01H501U,15/03/2026,Auto,450.000000,15/03/2027");

    bool eccezionePremio = false;
    try {
        Contratto invalido(2, 10, "RSSMRA80A01H501U", "15/03/2026", "Casa", -10.0, "15/03/2027");
    } catch (const std::invalid_argument& e) {
        eccezionePremio = true;
        std::string msg = e.what();
        assert(msg.find("remio") != std::string::npos); // "premio"/"Premio"
    }
    assert(eccezionePremio);

    bool eccezioneScadenza = false;
    try {
        Contratto invalido(3, 10, "RSSMRA80A01H501U", "15/03/2026", "Vita", 100.0, "31/02/2027");
    } catch (const std::invalid_argument&) {
        eccezioneScadenza = true;
    }
    assert(eccezioneScadenza);

    bool eccezioneTipoPolizza = false;
    try {
        Contratto invalido(4, 10, "RSSMRA80A01H501U", "15/03/2026", "", 100.0, "15/03/2027");
    } catch (const std::invalid_argument&) {
        eccezioneTipoPolizza = true;
    }
    assert(eccezioneTipoPolizza);

    std::cout << "test_contratto: tutti i test passati" << std::endl;
    return 0;
}
