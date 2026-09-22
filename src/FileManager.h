#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Rubrica.h"
#include "StoricoInterazioni.h"

// -----------------------------------------------------------------------------
// FileManager
// -----------------------------------------------------------------------------
// Unico punto del programma che legge e scrive file su disco. Tutte le altre
// classi lavorano solo con i dati in memoria (Rubrica, StoricoInterazioni).
//
// Formato usato: CSV (valori separati da virgola), una riga per record, con la
// prima riga di intestazione. Il caricamento e' "tollerante": se una riga e'
// rovinata viene saltata con un avviso e si continua con le altre.
//
// Sono funzioni libere dentro un namespace (non metodi di una classe) perche'
// non c'e' nessuno stato da conservare fra una chiamata e l'altra.
// -----------------------------------------------------------------------------
namespace FileManager {

    // Spezza una riga CSV nei suoi campi, gestendo i campi fra virgolette
    // (che possono contenere virgole) e le virgolette raddoppiate "".
    std::vector<std::string> parseRigaCSV(const std::string& riga);

    // --- Clienti ---
    void caricaClienti(Rubrica& rubrica, const std::string& path = "clienti.csv");   // valore di default: nome file standard del progetto
    void salvaClienti(const Rubrica& rubrica, const std::string& path = "clienti.csv");

    // --- Interazioni (appuntamenti e contratti stanno su due file distinti) ---
    void caricaInterazioni(StoricoInterazioni& storico,
                           const std::string& pathAppuntamenti = "appuntamenti.csv",
                           const std::string& pathContratti = "contratti.csv");
    void salvaInterazioni(const StoricoInterazioni& storico,
                          const std::string& pathAppuntamenti = "appuntamenti.csv",
                          const std::string& pathContratti = "contratti.csv");
}

#endif   // FILEMANAGER_H
