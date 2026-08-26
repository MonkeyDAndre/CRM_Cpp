#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../InsuraProCRM/Rubrica.h"

int main() {
    Rubrica rubrica;

    int id1 = rubrica.aggiungiCliente("Mario", "Rossi", "111", "mario@email.com",
                                       "trrndr98l12i483t", "01/01/1980");
    int id2 = rubrica.aggiungiCliente("Anna", "Rossetti", "222", "anna@email.com",
                                       "trrndr98l12i483t", "02/02/1985");
    int id3 = rubrica.aggiungiCliente("Luca", "Bianchi", "333", "luca@email.com",
                                       "trrndr98l12i483t", "03/03/1990");

    assert(id1 == 1);
    assert(id2 == 2);
    assert(id3 == 3);
    assert(rubrica.visualizzaTutti().size() == 3);

    Cliente c(0, "x", "x", "x", "x@x.com", "trrndr98l12i483t", "01/01/2000");
    assert(rubrica.getById(id2, c) == true);
    assert(c.getNome() == "Anna");

    Cliente nonUsato(0, "x", "x", "x", "x@x.com", "trrndr98l12i483t", "01/01/2000");
    assert(rubrica.getById(999, nonUsato) == false);
    assert(nonUsato.getNome() == "x"); // non toccato

    std::vector<Cliente> risultatoRicerca = rubrica.cercaPerNomeCognome("ross");
    assert(risultatoRicerca.size() == 2); // Rossi, Rossetti (case-insensitive)

    std::vector<Cliente> risultatoVuoto = rubrica.cercaPerNomeCognome("zzz");
    assert(risultatoVuoto.size() == 0);

    assert(rubrica.eliminaCliente(id2) == true);
    assert(rubrica.visualizzaTutti().size() == 2);
    assert(rubrica.getById(id2, c) == false);
    assert(rubrica.eliminaCliente(999) == false); // id inesistente, nessun crash

    bool eccezioneLanciata = false;
    try {
        rubrica.modificaCliente(id1, "Mario", "Rossi", "111", "email-non-valida",
                                 "trrndr98l12i483t", "01/01/1980");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);
    Cliente originale(0, "x", "x", "x", "x@x.com", "trrndr98l12i483t", "01/01/2000");
    rubrica.getById(id1, originale);
    assert(originale.getEmail() == "mario@email.com"); // non modificato dal tentativo fallito

    assert(rubrica.modificaCliente(999, "A", "B", "111", "a@b.com",
                                    "trrndr98l12i483t", "01/01/2000") == false);

    std::cout << "test_rubrica: tutti i test passati" << std::endl;
    return 0;
}
