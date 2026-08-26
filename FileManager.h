#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Rubrica.h"
#include "StoricoInterazioni.h"

namespace FileManager {
    std::vector<std::string> parseRigaCSV(const std::string& riga);

    void caricaClienti(Rubrica& rubrica, const std::string& path = "clienti.csv");
    void salvaClienti(const Rubrica& rubrica, const std::string& path = "clienti.csv");

    void caricaInterazioni(StoricoInterazioni& storico,
                            const std::string& pathAppuntamenti = "appuntamenti.csv",
                            const std::string& pathContratti = "contratti.csv");
    void salvaInterazioni(const StoricoInterazioni& storico,
                           const std::string& pathAppuntamenti = "appuntamenti.csv",
                           const std::string& pathContratti = "contratti.csv");
}

#endif