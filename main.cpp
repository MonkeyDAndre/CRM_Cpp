#include "Rubrica.h"
#include "StoricoInterazioni.h"
#include "FileManager.h"
#include "Menu.h"

int main() {
    Rubrica rubrica;
    StoricoInterazioni storico;

    FileManager::caricaClienti(rubrica);
    FileManager::caricaInterazioni(storico);

    Menu menu(rubrica, storico);
    menu.eseguiMenuPrincipale();

    return 0;
}