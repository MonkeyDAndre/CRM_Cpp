// =============================================================================
// CRM InsuraPro Solutions - punto di ingresso del programma
// =============================================================================
// Compilazione (nessuna libreria esterna):
//     g++ main.cpp src/*.cpp -o crm
//
// Flusso:
//   1. si creano le due strutture dati in memoria (Rubrica e StoricoInterazioni);
//   2. il FileManager le riempie leggendo i file CSV (se esistono);
//   3. si avvia il menu interattivo;
//   4. all'uscita dal menu il programma termina (il salvataggio su file avviene
//      dentro la voce "Esci" del menu).
// =============================================================================

#include "src/Rubrica.h"
#include "src/StoricoInterazioni.h"
#include "src/FileManager.h"
#include "src/Menu.h"

int main() {
    // Contenitori dei dati durante l'esecuzione: tutto il lavoro avviene qui in
    // memoria, i file servono solo per non perdere i dati fra un avvio e l'altro.
    Rubrica rubrica;                 // parte vuota, verra' riempita subito sotto se clienti.csv esiste
    StoricoInterazioni storico;      // idem, per appuntamenti/contratti

    // Caricamento iniziale da disco. Se i file non ci sono si parte vuoti.
    FileManager::caricaClienti(rubrica);       // legge clienti.csv (nome di default) e popola "rubrica"
    FileManager::caricaInterazioni(storico);   // legge appuntamenti.csv e contratti.csv e popola "storico"

    // Il menu lavora sugli stessi oggetti (passati per riferimento).
    Menu menu(rubrica, storico);   // il Menu non copia i dati, li referenzia soltanto
    menu.eseguiMenuPrincipale();   // qui il programma resta finche' l'utente non sceglie "Esci"

    return 0;   // codice 0 = terminazione senza errori
}
