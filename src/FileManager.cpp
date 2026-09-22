#include "FileManager.h"
#include "Appuntamento.h"
#include "Contratto.h"
#include <fstream>    // std::ifstream, std::ofstream
#include <iostream>   // std::cerr
#include <string>
#include <vector>

// =============================================================================
// parseRigaCSV: da una riga di testo a una lista di campi
// =============================================================================
// Regole CSV gestite:
//  - i campi sono separati da virgole;
//  - un campo puo' essere racchiuso fra virgolette " " e in tal caso puo'
//    contenere virgole senza che vengano interpretate come separatori;
//  - dentro un campo fra virgolette, due virgolette di fila ("") valgono una
//    virgoletta singola.
//
// Idea: scorriamo la riga carattere per carattere tenendo un interruttore
// "dentroVirgolette" che ci dice se in questo momento siamo o no dentro un
// campo quotato.
std::vector<std::string> FileManager::parseRigaCSV(const std::string& riga) {
    std::vector<std::string> campi;   // qui accumulo i campi trovati, nell'ordine in cui compaiono

    // Riga vuota: nessun campo da estrarre.
    if (riga.empty()) {
        return campi;   // vector vuoto, il chiamante decide come trattarlo
    }

    std::string campoCorrente;         // accumulatore del campo che sto costruendo carattere per carattere
    bool dentroVirgolette = false;     // interruttore: true mentre sto leggendo dentro un campo quotato

    for (size_t i = 0; i < riga.size(); ++i) {   // uso un indice (non range-for) perche' a volte devo "saltare" un carattere in piu'
        char c = riga[i];

        if (dentroVirgolette) {   // ramo A: sono dentro un campo fra virgolette
            if (c == '"') {
                // Una virgoletta seguita da un'altra virgoletta = virgoletta
                // letterale dentro il campo.
                if (i + 1 < riga.size() && riga[i + 1] == '"') {   // controllo il carattere successivo senza uscire dai limiti della stringa
                    campoCorrente += '"';   // e' una virgoletta "vera" dentro il testo, la tengo
                    ++i;                       // salto la seconda virgoletta
                } else {
                    dentroVirgolette = false;  // qui il campo quotato finisce
                }
            } else {
                campoCorrente += c;   // carattere qualsiasi dentro le virgolette (anche una virgola): lo tengo cosi' com'e'
            }
        } else {   // ramo B: sono FUORI da un campo quotato
            if (c == '"') {
                dentroVirgolette = true;       // inizia un campo quotato
            } else if (c == ',') {
                // Virgola "libera": chiude il campo corrente e ne apre uno nuovo.
                campi.push_back(campoCorrente);   // il campo letto finora e' completo, lo salvo
                campoCorrente.clear();             // riparto da zero per il campo successivo
            } else {
                campoCorrente += c;   // carattere normale, lo accumulo nel campo corrente
            }
        }
    }

    // L'ultimo campo non e' seguito da una virgola: lo aggiungiamo a mano.
    campi.push_back(campoCorrente);   // altrimenti l'ultimo campo andrebbe perso
    return campi;
}

// =============================================================================
// CLIENTI
// =============================================================================

// -----------------------------------------------------------------------------
// caricaClienti: legge clienti.csv e riempie la Rubrica
// -----------------------------------------------------------------------------
void FileManager::caricaClienti(Rubrica& rubrica, const std::string& path) {
    std::ifstream file(path);   // provo ad aprire il file in lettura
    if (!file.is_open()) {
        // File assente: non e' un errore. Si parte con la rubrica vuota e il
        // file verra' creato al primo salvataggio.
        return;   // esco subito, la rubrica resta com'era (vuota, al primo avvio)
    }

    std::string riga;
    bool primaRiga = true;   // la prima riga e' l'intestazione, va saltata
    int numeroRiga = 0;      // solo per i messaggi di avviso
    int maxId = 0;           // id piu' grande incontrato nel file

    while (std::getline(file, riga)) {   // leggo una riga alla volta finche' il file non finisce
        numeroRiga++;

        if (primaRiga) {
            primaRiga = false;
            continue;   // salto l'intestazione, non e' un cliente
        }
        if (riga.empty()) {
            continue;   // riga vuota (es. a fine file): la ignoro senza errori
        }

        std::vector<std::string> campi = parseRigaCSV(riga);   // spezzo la riga nei suoi 7 campi attesi
        if (campi.size() != 7) {
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (attesi 7 campi, trovati "
                      << campi.size() << ")" << std::endl;   // avviso su std::cerr, non blocco il caricamento
            continue;
        }

        try {
            // campi: 0=id 1=nome 2=cognome 3=telefono 4=email 5=cf 6=data
            int id = std::stoi(campi[0]);   // puo' lanciare std::invalid_argument/out_of_range se non e' un numero

            // La Rubrica assegna gli id da sola partendo da "prossimoId_".
            // Per conservare l'id gia' scritto nel file, impostiamo il
            // contatore a quel valore appena prima di chiamare aggiungiCliente:
            // cosi' il cliente riceve esattamente "id".
            rubrica.impostaProssimoId(id);   // trucco per riusare aggiungiCliente senza duplicare la logica di creazione
            rubrica.aggiungiCliente(campi[1], campi[2], campi[3],
                                    campi[4], campi[5], campi[6]);   // puo' lanciare se un campo non passa la validazione

            if (id > maxId) {
                maxId = id;   // tengo traccia dell'id piu' alto visto finora, mi servira' a fine funzione
            }
        } catch (const std::exception& e) {
            // Puo' arrivare qui per un id non numerico (std::stoi) o per un
            // campo che non supera la validazione (costruttore di Cliente).
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            continue;   // riga scartata, ma il caricamento delle righe successive continua
        }
    }

    // Il prossimo cliente creato dall'utente avra' un id nuovo, subito dopo il
    // massimo trovato nel file.
    rubrica.impostaProssimoId(maxId + 1);   // se il file era vuoto, maxId resta 0 e si riparte da 1 (comportamento corretto)
}

// -----------------------------------------------------------------------------
// salvaClienti: riscrive completamente clienti.csv con i dati in memoria
// -----------------------------------------------------------------------------
void FileManager::salvaClienti(const Rubrica& rubrica, const std::string& path) {
    // std::ios::trunc: se il file esiste gia' viene svuotato e riscritto da zero.
    std::ofstream file(path, std::ios::trunc);   // apro (o creo) il file in scrittura, azzerando il contenuto precedente
    if (!file.is_open()) {
        std::cerr << "Errore: impossibile scrivere su " << path << std::endl;   // es. permessi negati, disco pieno...
        return;   // i dati in memoria restano intatti, semplicemente non vengono salvati
    }

    file << "id,nome,cognome,telefono,email,codice_fiscale,data_nascita\n";   // riga di intestazione, letta e saltata da caricaClienti
    for (const Cliente& c : rubrica.visualizzaTutti()) {   // scrivo una riga per ogni cliente attualmente in memoria
        file << c.getId() << ","
             << c.getNome() << ","
             << c.getCognome() << ","
             << c.getTelefono() << ","
             << c.getEmail() << ","
             << c.getCodiceFiscale() << ","
             << c.getDataNascita() << "\n";   // nessun campo cliente contiene virgole per costruzione, quindi niente escape qui
    }
}

// =============================================================================
// INTERAZIONI
// =============================================================================

// -----------------------------------------------------------------------------
// caricaInterazioni: legge appuntamenti.csv e contratti.csv
// -----------------------------------------------------------------------------
// I due file hanno struttura diversa, quindi li leggiamo con due cicli
// separati. Appuntamenti e contratti condividono lo stesso contatore di id:
// alla fine impostiamo "prossimoId_" al massimo id trovato fra i due file + 1.
void FileManager::caricaInterazioni(StoricoInterazioni& storico,
                                    const std::string& pathAppuntamenti,
                                    const std::string& pathContratti) {
    int maxId = 0;   // massimo id visto fra ENTRAMBI i file, perche' il contatore e' condiviso

    // --- File degli appuntamenti ---
    std::ifstream fileApp(pathAppuntamenti);   // provo ad aprirlo, se manca semplicemente salto questo blocco
    if (fileApp.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;

        while (std::getline(fileApp, riga)) {   // stesso schema di lettura riga-per-riga visto in caricaClienti
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }   // salto l'intestazione
            if (riga.empty()) { continue; }                    // salto righe vuote

            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }

            try {
                // campi: 0=id 1=id_cliente 2=cf 3=data 4=orario 5=esito 6=note
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);

                // Stesso trucco usato per i clienti: impostiamo il contatore
                // all'id del file cosi' l'interazione lo conserva.
                storico.impostaProssimoId(id);
                storico.aggiungiAppuntamento(idCliente, campi[2], campi[3],
                                             campi[4], campi[5], campi[6]);   // puo' lanciare se un campo non e' valido

                if (id > maxId) { maxId = id; }
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    // --- File dei contratti ---
    std::ifstream fileContr(pathContratti);   // apro anche il secondo file, indipendentemente dal primo
    if (fileContr.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;

        while (std::getline(fileContr, riga)) {
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }
            if (riga.empty()) { continue; }

            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }

            try {
                // campi: 0=id 1=id_cliente 2=cf 3=data 4=tipo_polizza 5=premio 6=data_scadenza
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);
                double premio = std::stod(campi[5]);   // lancia se non numerico

                storico.impostaProssimoId(id);
                storico.aggiungiContratto(idCliente, campi[2], campi[3],
                                          campi[4], premio, campi[6]);

                if (id > maxId) { maxId = id; }   // stesso contatore "maxId" usato sopra per gli appuntamenti: e' voluto
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    storico.impostaProssimoId(maxId + 1);   // fissato UNA sola volta, dopo aver letto entrambi i file
}

// -----------------------------------------------------------------------------
// salvaInterazioni: riscrive appuntamenti.csv e contratti.csv
// -----------------------------------------------------------------------------
void FileManager::salvaInterazioni(const StoricoInterazioni& storico,
                                   const std::string& pathAppuntamenti,
                                   const std::string& pathContratti) {
    std::ofstream fileApp(pathAppuntamenti, std::ios::trunc);    // riscrivo da zero entrambi i file
    std::ofstream fileContr(pathContratti, std::ios::trunc);
    if (!fileApp.is_open() || !fileContr.is_open()) {   // se anche solo uno dei due non si apre, meglio non scrivere niente a meta'
        std::cerr << "Errore: impossibile scrivere i file delle interazioni" << std::endl;
        return;
    }

    fileApp << "id,id_cliente,codice_fiscale,data,orario,esito,note\n";                    // intestazione appuntamenti
    fileContr << "id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza\n";  // intestazione contratti

    // Scorriamo l'unica lista globale e mandiamo ogni interazione nel file
    // giusto, riconoscendola dal suo tipo. toCSV() e' polimorfico: produce la
    // riga nel formato corretto a seconda che sia Appuntamento o Contratto.
    for (const std::unique_ptr<Interazione>& interazione : storico.tutte()) {   // un solo ciclo su tutta la lista, non due liste separate
        if (interazione->getTipo() == "Appuntamento") {
            fileApp << interazione->toCSV() << "\n";   // toCSV() chiama la versione giusta grazie al polimorfismo
        } else if (interazione->getTipo() == "Contratto") {
            fileContr << interazione->toCSV() << "\n";
        }
        // nota per me: non c'e' un "else" perche' al momento le uniche due
        // figlie concrete di Interazione sono Appuntamento e Contratto
    }
}
