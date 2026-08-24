#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../InsuraProCRM/Cliente.h"

int main() {
    Cliente c(1, "Mario", "Rossi", "3331234567", "mario.rossi@email.com",
              "RSSMRA80A01H501U", "01/01/1980");

    assert(c.getId() == 1);
    assert(c.getNome() == "Mario");
    assert(c.getCognome() == "Rossi");
    assert(c.getEmail() == "mario.rossi@email.com");
    assert(c.getCodiceFiscale() == "RSSMRA80A01H501U");
    assert(c.getDataNascita() == "01/01/1980");

    // setter con valore non valido: deve lanciare e non modificare il campo
    bool eccezioneLanciata = false;
    try {
        c.setEmail("non-valida");
    } catch (const std::invalid_argument& e) {
        eccezioneLanciata = true;
        std::string messaggio = e.what();
        assert(messaggio.find("mail") != std::string::npos ||
               messaggio.find("Email") != std::string::npos);
    }
    assert(eccezioneLanciata);
    assert(c.getEmail() == "mario.rossi@email.com"); // valore precedente intatto

    eccezioneLanciata = false;
    try {
        c.setCodiceFiscale("TROPPO_CORTO");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);
    assert(c.getCodiceFiscale() == "RSSMRA80A01H501U");

    // costruttore con dato non valido deve lanciare
    bool costruttoreLancia = false;
    try {
        Cliente invalido(2, "Luca", "Bianchi", "123", "email-invalida",
                          "CORTO", "31/02/2020");
    } catch (const std::invalid_argument&) {
        costruttoreLancia = true;
    }
    assert(costruttoreLancia);

    std::cout << "test_cliente: tutti i test passati" << std::endl;
    return 0;
}
