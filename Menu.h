#ifndef MENU_H
#define MENU_H

#include "Rubrica.h"
#include "StoricoInterazioni.h"

class Menu {
public:
    Menu(Rubrica& rubrica, StoricoInterazioni& storico);

    void eseguiMenuPrincipale();

private:
    Rubrica& rubrica_;
    StoricoInterazioni& storico_;

    void eseguiSottomenuClienti();
    void aggiungiClienteInterattivo();
    void visualizzaTuttiIClienti();
    void modificaClienteInterattivo();
    void eliminaClienteInterattivo();
    void cercaClienteInterattivo();

    void eseguiSottomenuInterazioni();
    void aggiungiAppuntamentoInterattivo();
    void aggiungiContrattoInterattivo();
    void visualizzaInterazioniClienteInterattivo();
    void cercaInterazioniInterattivo();

    void salvaESci();

    static int leggiSceltaMenu();
    static std::string leggiRiga(const std::string& prompt);
};

#endif