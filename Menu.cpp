#include "Menu.h"
#include <iostream>  
#include <stdexcept>
#include <limits>
#include "FileManager.h"

Menu::Menu(Rubrica& rubrica, StoricoInterazioni& storico)
    : rubrica_(rubrica), storico_(storico) {}

std::string Menu::leggiRiga(const std::string& prompt) {
    std::cout << prompt;
    std::string valore;
    std::getline(std::cin, valore);
    return valore;
}

int Menu::leggiSceltaMenu() {
    std::string riga;
    std::getline(std::cin, riga);
    try {
        size_t pos;
        int valore = std::stoi(riga, &pos);
        if (pos != riga.size()) return -1; // caratteri extra dopo il numero
        return valore;
    } catch (const std::exception&) {
        return -1;
    }
}

void Menu::aggiungiClienteInterattivo() {
    std::string nome = leggiRiga("Nome: ");
    std::string cognome = leggiRiga("Cognome: ");
    std::string telefono = leggiRiga("Telefono: ");
    std::string email = leggiRiga("Email: ");;
    std::string codiceFiscale = leggiRiga("Codice fiscale: ");
    std::string dataNascita = leggiRiga("Data di nascita (gg/mm/aaaa): ");

    while (true) {
        try {
            rubrica_.aggiungiCliente(nome, cognome, telefono, email, codiceFiscale, dataNascita);
            std::cout << "Cliente aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Email") != std::string::npos) {
                email = leggiRiga("Reinserisci Email: ");
            } else if (messaggio.find("fiscale") != std::string::npos) {
                codiceFiscale = leggiRiga("Reinserisci codice fiscale: ");
            } else if (messaggio.find("nascita") != std::string::npos) {
                dataNascita = leggiRiga("Reinserisci data di nascita: ");
            } else if (messaggio.find("Nome") != std::string::npos) {
                nome = leggiRiga("Reinserisci nome: ");
            } else if (messaggio.find("Cognome") != std::string::npos) {
                cognome = leggiRiga("Reinserisci cognome: ");
            } else if (messaggio.find("Telefono") != std::string::npos) {
                telefono = leggiRiga("Reinserisci telefono: ");
            } else {
                nome = leggiRiga("Reinserisci nome: "); // fallback difensivo
            }
        }
    }
}

void Menu::visualizzaTuttiIClienti() {
    const auto& clienti = rubrica_.visualizzaTutti();
    if (clienti.empty()) {
        std::cout << "Nessun cliente presente." << std::endl;
        return;
    }
    for (const auto& c : clienti) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << " - " << c.getEmail() << " - " << c.getTelefono() << std::endl;
    }
}

void Menu::modificaClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente da modificare: ");
    int id;
    try {
        id = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    Cliente esistente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(id, esistente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string nome = leggiRiga("Nuovo nome [" + esistente.getNome() + "]: ");
    if (nome.empty()) nome = esistente.getNome();
    std::string cognome = leggiRiga("Nuovo cognome [" + esistente.getCognome() + "]: ");
    if (cognome.empty()) cognome = esistente.getCognome();
    std::string telefono = leggiRiga("Nuovo telefono [" + esistente.getTelefono() + "]: ");
    if (telefono.empty()) telefono = esistente.getTelefono();
    std::string email = leggiRiga("Nuova email [" + esistente.getEmail() + "]: ");
    if (email.empty()) email = esistente.getEmail();
    std::string cf = leggiRiga("Nuovo codice fiscale [" + esistente.getCodiceFiscale() + "]: ");
    if (cf.empty()) cf = esistente.getCodiceFiscale();
    std::string dataNascita = leggiRiga("Nuova data nascita [" + esistente.getDataNascita() + "]: ");
    if (dataNascita.empty()) dataNascita = esistente.getDataNascita();

    try {
        rubrica_.modificaCliente(id, nome, cognome, telefono, email, cf, dataNascita);
        std::cout << "Cliente modificato con successo." << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Errore: " << e.what() << std::endl;
    }
}

void Menu::eliminaClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente da eliminare: ");
    int id;
    try {
        id = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    if (rubrica_.eliminaCliente(id)) {
        std::cout << "Cliente eliminato." << std::endl;
    } else {
        std::cout << "Cliente non trovato." << std::endl;
    }
}

void Menu::cercaClienteInterattivo() {
    std::string query = leggiRiga("Nome o cognome da cercare: ");
    auto risultati = rubrica_.cercaPerNomeCognome(query);
    if (risultati.empty()) {
        std::cout << "Nessun cliente trovato." << std::endl;
        return;
    }
    for (const auto& c : risultati) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << std::endl;
    }
}

void Menu::eseguiSottomenuClienti() {
    while (true) {
        std::cout << "\n1. Aggiungi cliente\n2. Visualizza tutti i clienti\n"
                  << "3. Modifica cliente\n4. Elimina cliente\n5. Cerca cliente\n"
                  << "0. Torna al menu principale\nScelta: ";
        int scelta = leggiSceltaMenu();
        switch (scelta) {
            case 1: aggiungiClienteInterattivo(); break;
            case 2: visualizzaTuttiIClienti(); break;
            case 3: modificaClienteInterattivo(); break;
            case 4: eliminaClienteInterattivo(); break;
            case 5: cercaClienteInterattivo(); break;
            case 0: return;
            default: std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}

void Menu::aggiungiAppuntamentoInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(idCliente, cliente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string data = leggiRiga("Data (gg/mm/aaaa): ");
    std::string orario = leggiRiga("Orario (hh:mm): ");
    std::string esito = leggiRiga("Esito: ");
    std::string note = leggiRiga("Note (facoltative): ");

    while (true) {
        try {
            storico_.aggiungiAppuntamento(idCliente, cliente.getCodiceFiscale(), data, orario,
                                           esito, note);
            std::cout << "Appuntamento aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Data") != std::string::npos) {
                data = leggiRiga("Reinserisci data: ");
            } else if (messaggio.find("Orario") != std::string::npos) {
                orario = leggiRiga("Reinserisci orario: ");
            } else if (messaggio.find("Esito") != std::string::npos) {
                esito = leggiRiga("Reinserisci esito: ");
            } else {
                data = leggiRiga("Reinserisci data: "); // fallback difensivo
            }
        }
    }
}

void Menu::aggiungiContrattoInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(idCliente, cliente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string data = leggiRiga("Data (gg/mm/aaaa): ");
    std::string tipoPolizza = leggiRiga("Tipo polizza: ");
    std::string premioStr = leggiRiga("Premio: ");
    std::string dataScadenza = leggiRiga("Data scadenza (gg/mm/aaaa): ");

    while (true) {
        double premio;
        try {
            premio = std::stod(premioStr);
        } catch (const std::exception&) {
            std::cout << "Errore: Premio non numerico." << std::endl;
            premioStr = leggiRiga("Reinserisci premio: ");
            continue;
        }
        try {
            storico_.aggiungiContratto(idCliente, cliente.getCodiceFiscale(), data, tipoPolizza,
                                        premio, dataScadenza);
            std::cout << "Contratto aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Premio") != std::string::npos) {
                premioStr = leggiRiga("Reinserisci premio: ");
            } else if (messaggio.find("scadenza") != std::string::npos) {
                dataScadenza = leggiRiga("Reinserisci data scadenza: ");
            } else if (messaggio.find("polizza") != std::string::npos) {
                tipoPolizza = leggiRiga("Reinserisci tipo polizza: ");
            } else {
                data = leggiRiga("Reinserisci data: "); // fallback difensivo
            }
        }
    }
}

void Menu::visualizzaInterazioniClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    // Nota: NON si richiede che il cliente esista ancora nella Rubrica: le
    // interazioni di un cliente eliminato restano consultabili (vedi design,
    // sezione "Eliminazione cliente").
    auto interazioni = storico_.getPerCliente(idCliente, "");
    if (interazioni.empty()) {
        std::cout << "Nessuna interazione trovata per questo cliente." << std::endl;
        return;
    }
    for (const auto* i : interazioni) {
        i->stampa();
    }
}

void Menu::cercaInterazioniInterattivo() {
    std::string data = leggiRiga("Cerca per data (gg/mm/aaaa, vuoto per saltare): ");
    for (const auto& interazione : storico_.tutte()) {
        if (data.empty() || interazione->getData() == data) {
            interazione->stampa();
        }
    }
}

void Menu::eseguiSottomenuInterazioni() {
    while (true) {
        std::cout << "\n1. Aggiungi appuntamento\n2. Aggiungi contratto\n"
                  << "3. Visualizza interazioni di un cliente\n4. Cerca interazioni\n"
                  << "0. Torna al menu principale\nScelta: ";
        int scelta = leggiSceltaMenu();
        switch (scelta) {
            case 1: aggiungiAppuntamentoInterattivo(); break;
            case 2: aggiungiContrattoInterattivo(); break;
            case 3: visualizzaInterazioniClienteInterattivo(); break;
            case 4: cercaInterazioniInterattivo(); break;
            case 0: return;
            default: std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}

void Menu::salvaESci() {
    FileManager::salvaClienti(rubrica_);
    FileManager::salvaInterazioni(storico_);
    std::cout << "Dati salvati. Uscita in corso." << std::endl;
}

void Menu::eseguiMenuPrincipale() {
    while (true) {
        std::cout << "\nMENU PRINCIPALE\n1. Gestione Clienti\n2. Gestione Interazioni\n"
                  << "3. Esci\nScelta: ";
        int scelta = leggiSceltaMenu();
        if (scelta == 1) {
            eseguiSottomenuClienti();
        } else if (scelta == 2) {
            eseguiSottomenuInterazioni();
        } else if (scelta == 3) {
            salvaESci();
            return;
        } else {
            std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}