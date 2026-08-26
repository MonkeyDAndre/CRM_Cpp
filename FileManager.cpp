#include "Appuntamento.h"
#include "Contratto.h"
#include "FileManager.h"
#include "StoricoInterazioni.h"
#include <fstream>   
#include <iostream>  
#include <ostream>   
// (aggiungere questi include in cima al file, insieme a quelli gia' presenti dal Task 8)

void FileManager::caricaInterazioni(StoricoInterazioni& storico,
                                     const std::string& pathAppuntamenti,
                                     const std::string& pathContratti) {
    int maxId = 0;

    std::ifstream fileApp(pathAppuntamenti);
    if (fileApp.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;
        while (std::getline(fileApp, riga)) {
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }
            if (riga.empty()) continue;
            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }
            try {
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);
                storico.impostaProssimoId(id);
                storico.aggiungiAppuntamento(idCliente, campi[2], campi[3], campi[4], campi[5],
                                              campi[6]);
                if (id > maxId) maxId = id;
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    std::ifstream fileContr(pathContratti);
    if (fileContr.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;
        while (std::getline(fileContr, riga)) {
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }
            if (riga.empty()) continue;
            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }
            try {
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);
                double premio = std::stod(campi[5]); // lancia std::invalid_argument se non numerico
                storico.impostaProssimoId(id);
                storico.aggiungiContratto(idCliente, campi[2], campi[3], campi[4], premio,
                                           campi[6]);
                if (id > maxId) maxId = id;
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    storico.impostaProssimoId(maxId + 1);
}

void FileManager::salvaInterazioni(const StoricoInterazioni& storico,
                                    const std::string& pathAppuntamenti,
                                    const std::string& pathContratti) {
    std::ofstream fileApp(pathAppuntamenti, std::ios::trunc);
    std::ofstream fileContr(pathContratti, std::ios::trunc);
    if (!fileApp.is_open() || !fileContr.is_open()) {
        std::cerr << "Errore: impossibile scrivere i file delle interazioni" << std::endl;
        return;
    }

    fileApp << "id,id_cliente,codice_fiscale,data,orario,esito,note\n";
    fileContr << "id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza\n";

    for (const auto& interazione : storico.tutte()) {
        if (interazione->getTipo() == "Appuntamento") {
            fileApp << interazione->toCSV() << "\n";
        } else if (interazione->getTipo() == "Contratto") {
            fileContr << interazione->toCSV() << "\n";
        }
    }
}

void FileManager::caricaClienti(Rubrica& rubrica, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return; // file assente non e' un errore
    }

    std::string riga;
    bool primaRiga = true;
    int numeroRiga = 0;
    int maxId = 0;

    while (std::getline(file, riga)) {
        numeroRiga++;
        if (primaRiga) { primaRiga = false; continue; } // intestazione
        if (riga.empty()) continue;

        std::vector<std::string> campi = parseRigaCSV(riga);
        if (campi.size() != 7) {
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (attesi 7 campi, trovati "
                      << campi.size() << ")" << std::endl;
            continue;
        }

        try {
            int id = std::stoi(campi[0]);
            Cliente c(id, campi[1], campi[2], campi[3], campi[4], campi[5], campi[6]);
            // Rubrica assegna i propri id in aggiungiCliente(); per rispettare l'id
            // gia' presente nel file, impostiamo il contatore e usiamo un percorso
            // dedicato che non rigenera l'id: aggiungiClienteConId non esiste ancora
            // nell'interfaccia Rubrica, quindi qui si ricostruisce lo stato tramite
            // aggiungiCliente() e si forza l'id atteso prima di richiamarlo:
            rubrica.impostaProssimoId(id);
            rubrica.aggiungiCliente(campi[1], campi[2], campi[3], campi[4], campi[5], campi[6]);
            if (id > maxId) maxId = id;
        } catch (const std::exception& e) {
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            continue;
        }
    }

    rubrica.impostaProssimoId(maxId + 1);
}

void FileManager::salvaClienti(const Rubrica& rubrica, const std::string& path) {
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Errore: impossibile scrivere su " << path << std::endl;
        return;
    }

    file << "id,nome,cognome,telefono,email,codice_fiscale,data_nascita\n";
    for (const auto& c : rubrica.visualizzaTutti()) {
        file << c.getId() << "," << c.getNome() << "," << c.getCognome() << ","
             << c.getTelefono() << "," << c.getEmail() << "," << c.getCodiceFiscale() << ","
             << c.getDataNascita() << "\n";
    }
}

std::vector<std::string> FileManager::parseRigaCSV(const std::string& riga) {
    std::vector<std::string> campi;
    if (riga.empty()) return campi;

    std::string campoCorrente;
    bool dentroVirgolette = false;

    for (size_t i = 0; i < riga.size(); ++i) {
        char c = riga[i];
        if (dentroVirgolette) {
            if (c == '"') {
                if (i + 1 < riga.size() && riga[i + 1] == '"') {
                    campoCorrente += '"';
                    ++i; // salta il secondo '"' dell'escape
                } else {
                    dentroVirgolette = false;
                }
            } else {
                campoCorrente += c;
            }
        } else {
            if (c == '"') {
                dentroVirgolette = true;
            } else if (c == ',') {
                campi.push_back(campoCorrente);
                campoCorrente.clear();
            } else {
                campoCorrente += c;
            }
        }
    }
    campi.push_back(campoCorrente);
    return campi;
}