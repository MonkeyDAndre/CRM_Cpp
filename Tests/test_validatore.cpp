#include <cassert>
#include <iostream>
#include "../InsuraProCRM/Validatore.h"

int main() {
    // emailValida
    assert(Validatore::emailValida("a@b.com") == true);
    assert(Validatore::emailValida("abc") == false);
    assert(Validatore::emailValida("a@bcom") == false);
    assert(Validatore::emailValida("") == false);

    // codiceFiscaleValido
    assert(Validatore::codiceFiscaleValido("TRRNDR98L13i483t") == true); // 16 alfanumerici
    assert(Validatore::codiceFiscaleValido("RSSMRA80A01H501") == false); // 15 caratteri
    assert(Validatore::codiceFiscaleValido("RSSMRA80A01H501!") == false); // simbolo non alfanumerico

    // dataValida
    assert(Validatore::dataValida("29/02/2024") == true);  // bisestile
    assert(Validatore::dataValida("29/02/2023") == false); // non bisestile
    assert(Validatore::dataValida("31/04/2026") == false); // aprile ha 30 giorni
    assert(Validatore::dataValida("32/01/2026") == false); // giorno inesistente
    assert(Validatore::dataValida("15/13/2026") == false); // mese inesistente
    assert(Validatore::dataValida("01/01/2026") == true);

    // numeroPositivo
    assert(Validatore::numeroPositivo(0.0) == true);
    assert(Validatore::numeroPositivo(-1.0) == false);
    assert(Validatore::numeroPositivo(0.01) == true);

    std::cout << "test_validatore: tutti i test passati" << std::endl;

    /*
    // campoNonVuoto (trim su spazi)
    assert(Validatore::campoNonVuoto("") == false);
    assert(Validatore::campoNonVuoto("   ") == false);
    assert(Validatore::campoNonVuoto("  a  ") == true);
    assert(Validatore::campoNonVuoto("Mario") == true);
    */

    
    return 0;
}
