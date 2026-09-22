#include "Menu.h"
#include "FileManager.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>   // std::invalid_argument, std::stoi, std::stod, std::out_of_range
#include <cstdlib>     // std::system, usato da pulisciSchermo
#include <cctype>      // std::isdigit

namespace {
    // Digitando esattamente questo token in un campo, l'utente annulla
    // l'inserimento in corso e torna al menu da cui e' partito.
    const std::string TOKEN_ANNULLA = "&";

    // Eccezione "segnale", vuota di proposito: non deriva da std::exception
    // ne' da std::invalid_argument, cosi' i catch gia' presenti nel codice
    // (pensati per errori di validazione) non la intercettano per sbaglio.
    // Viene lanciata da leggiRiga() e risale la pila di chiamate fino al
    // catch dedicato nello switch dei sottomenu.
    struct OperazioneAnnullata {};
}

// -----------------------------------------------------------------------------
// Costruttore: salva i riferimenti a Rubrica e StoricoInterazioni
// -----------------------------------------------------------------------------
Menu::Menu(Rubrica& rubrica, StoricoInterazioni& storico)
    : rubrica_(rubrica), storico_(storico) {}   // i riferimenti vanno inizializzati qui, non si possono assegnare dopo

// =============================================================================
// Aiutanti per l'input
// =============================================================================

// Stampa il messaggio "prompt" (senza a-capo) e legge l'intera riga digitata.
// Se l'utente digita esattamente il token di annullamento, lancia
// OperazioneAnnullata invece di ritornare: chi ha chiamato leggiRiga() non
// riceve mai quel valore come se fosse un dato valido.
std::string Menu::leggiRiga(const std::string& prompt) {
    std::cout << prompt;   // niente std::endl qui: voglio che l'utente digiti sulla stessa riga del prompt
    std::string valore;
    std::getline(std::cin, valore);   // getline (non cin >>) per prendere l'intera riga, spazi compresi
    if (valore == TOKEN_ANNULLA) {
        throw OperazioneAnnullata();
    }
    return valore;
}

// Legge una riga e prova a interpretarla come numero intero.
// Ritorna -1 se la riga, dopo aver tolto gli spazi ai bordi, non e' composta
// esclusivamente da cifre. Chi chiama trattera' -1 come "scelta non valida".
int Menu::leggiSceltaMenu() {
    std::string riga;
    std::getline(std::cin, riga);   // leggo la riga intera
    riga = Validatore::trim(riga);  // tolgo spazi/tab/a-capo ai bordi prima di controllare le cifre

    if (riga.empty()) {
        return -1;   // riga vuota o di soli spazi: nessuna cifra da leggere
    }
    for (char c : riga) {
        // static_cast a unsigned char: passare un char "negativo" (es. lettere
        // accentate) a isdigit e' undefined behaviour senza questo cast.
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return -1;   // almeno un carattere non e' una cifra (es. "3abc", "-3")
        }
    }

    try {
        return std::stoi(riga);   // qui riga e' garantita fatta di sole cifre, ma potrebbe essere troppo grande per un int
    } catch (const std::out_of_range&) {
        return -1;   // es. "99999999999999": tutte cifre, ma fuori dal range di int
    }
}

// =============================================================================
// Presentazione schermate
// =============================================================================

// Pulisce il terminale: comando diverso a seconda del sistema operativo,
// scelto a tempo di compilazione. Le stringhe sono letterali (nessun input
// utente coinvolto), quindi std::system qui non comporta rischi di command
// injection.
void Menu::pulisciSchermo() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// Pulisce lo schermo e stampa il titolo della schermata corrente, cosi'
// ogni punto di chiamata mostra solo l'informazione rilevante in quel momento.
// Se permetteAnnulla e' true, ricorda all'utente come tornare indietro.
void Menu::mostraSchermata(const std::string& titolo, bool permetteAnnulla) {
    pulisciSchermo();
    std::cout << "=== " << titolo << " ===" << std::endl;
    if (permetteAnnulla) {
        std::cout << "(digita " << TOKEN_ANNULLA
                  << " e premi invio per tornare al menu precedente)" << std::endl;
    }
    std::cout << std::endl;
}

// Aspetta che l'utente prema Invio prima di continuare, cosi' il risultato
// di un'azione (messaggio di errore/successo, elenco stampato...) resta
// visibile invece di sparire non appena il menu chiamante si ripulisce.
void Menu::attendiInvio() {
    std::cout << "\nPremi INVIO per continuare...";
    std::string scarto;
    std::getline(std::cin, scarto);   // il valore letto non serve, ci interessa solo aspettare
}

// =============================================================================
// MENU PRINCIPALE
// =============================================================================
void Menu::eseguiMenuPrincipale() {
    while (true) {   // ciclo infinito: l'unico modo per uscire e' la scelta "3" (vedi return piu' sotto)
        mostraSchermata("MENU PRINCIPALE");
        std::cout << "1. Gestione Clienti\n"
                  << "2. Gestione Interazioni\n"
                  << "3. Esci\n"
                  << "Scelta: ";

        int scelta = leggiSceltaMenu();   // -1 se l'utente ha digitato qualcosa di non valido
        if (scelta == 1) {
            eseguiSottomenuClienti();   // delego tutto il sottomenu clienti, poi torno qui e ristampo il menu principale
        } else if (scelta == 2) {
            eseguiSottomenuInterazioni();
        } else if (scelta == 3) {
            salvaESci();   // salvo su disco prima di terminare
            return;                 // unica via d'uscita dal programma
        } else {
            std::cout << "Scelta non valida, riprova." << std::endl;   // qualunque altro numero, o -1: ripropongo il menu
        }
    }
}

// =============================================================================
// SOTTOMENU CLIENTI
// =============================================================================
void Menu::eseguiSottomenuClienti() {
    while (true) {   // resto in questo sottomenu finche' l'utente non sceglie "0" (torna indietro)
        mostraSchermata("GESTIONE CLIENTI");
        std::cout << "1. Aggiungi cliente\n"
                  << "2. Visualizza tutti i clienti\n"
                  << "3. Modifica cliente\n"
                  << "4. Elimina cliente\n"
                  << "5. Cerca cliente\n"
                  << "0. Torna al menu principale\n"
                  << "Scelta: ";

        int scelta = leggiSceltaMenu();
        try {
            switch (scelta) {   // switch invece di if/else a catena: piu' leggibile con tante opzioni numeriche
                case 1: aggiungiClienteInterattivo();  attendiInvio(); break;
                case 2: visualizzaTuttiIClienti();     attendiInvio(); break;
                case 3: modificaClienteInterattivo();  attendiInvio(); break;
                case 4: eliminaClienteInterattivo();   attendiInvio(); break;
                case 5: cercaClienteInterattivo();     attendiInvio(); break;
                case 0: return;                        // torna al menu principale
                default: std::cout << "Scelta non valida, riprova." << std::endl;   // copre sia -1 sia numeri fuori range
            }
        } catch (const OperazioneAnnullata&) {   // l'utente ha digitato il token di annullamento durante un campo
            std::cout << "Operazione annullata." << std::endl;
            attendiInvio();
        }
    }
}

// -----------------------------------------------------------------------------
// Aggiungi cliente
// -----------------------------------------------------------------------------
// Chiediamo tutti i campi, poi proviamo a creare il cliente. Se un campo non
// passa la validazione, la Rubrica lancia std::invalid_argument: leggiamo il
// messaggio, capiamo QUALE campo era sbagliato, lo richiediamo e riproviamo.
// Gli altri campi gia' inseriti non vengono persi.
void Menu::aggiungiClienteInterattivo() {
    mostraSchermata("AGGIUNGI CLIENTE", true);
    std::string nome = leggiRiga("Nome: ");                           // prima raccolta di TUTTI i campi, senza validare ancora nulla
    std::string cognome = leggiRiga("Cognome: ");
    std::string telefono = leggiRiga("Telefono: ");
    std::string email = leggiRiga("Email: ");
    std::string codiceFiscale = leggiRiga("Codice fiscale: ");
    std::string dataNascita = leggiRiga("Data di nascita (gg/mm/aaaa): ");

    while (true) {   // riprovo finche' aggiungiCliente non va a buon fine
        try {
            rubrica_.aggiungiCliente(nome, cognome, telefono, email,
                                     codiceFiscale, dataNascita);   // qui scattano tutte le validazioni di Cliente
            std::cout << "Cliente aggiunto con successo." << std::endl;
            return;                 // inserimento riuscito: usciamo dal ciclo
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;   // mostro all'utente il motivo esatto del rifiuto

            // Il messaggio di errore contiene il nome del campo: cerchiamo
            // quella parola per decidere cosa far reinserire.
            std::string messaggio = e.what();
            if (messaggio.find("Email") != std::string::npos) {
                email = leggiRiga("Reinserisci Email: ");   // riprovo solo il campo incriminato, gli altri restano quelli buoni
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
                // Caso imprevisto: per sicurezza richiediamo il nome.
                nome = leggiRiga("Reinserisci nome: ");   // fallback, non dovrebbe mai capitare con i messaggi attuali di Cliente
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Visualizza tutti i clienti
// -----------------------------------------------------------------------------
void Menu::visualizzaTuttiIClienti() {
    mostraSchermata("VISUALIZZAZIONE CONTATTI");
    stampaListaClienti(rubrica_.visualizzaTutti());   // riferimento: nessuna copia dell'intera rubrica
}

// Stampa un cliente per riga, nel formato "#id nome cognome - email - telefono".
// Usata sia da "Visualizza tutti i clienti" sia dalle schermate che chiedono
// un ID (modifica/elimina), per far vedere subito tra chi scegliere.
void Menu::stampaListaClienti(const std::vector<Cliente>& clienti) {
    if (clienti.empty()) {
        std::cout << "Nessun cliente presente." << std::endl;
        return;
    }
    for (const Cliente& c : clienti) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << " - " << c.getEmail() << " - " << c.getTelefono() << std::endl;
    }
}

// -----------------------------------------------------------------------------
// Modifica cliente
// -----------------------------------------------------------------------------
// Per ogni campo mostriamo il valore attuale fra parentesi quadre: se l'utente
// preme solo Invio (riga vuota) teniamo il valore vecchio.
void Menu::modificaClienteInterattivo() {
    mostraSchermata("MODIFICA CLIENTE", true);
    stampaListaClienti(rubrica_.visualizzaTutti());   // cosi' l'utente vede subito gli id tra cui scegliere
    std::cout << std::endl;
    std::string idStr = leggiRiga("ID cliente da modificare: ");
    int id;
    try {
        id = std::stoi(idStr);   // converto il testo digitato in un numero
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;   // idStr non era un numero: niente da fare, esco dalla funzione
        return;
    }

    // getById vuole un Cliente da riempire: ne creiamo uno "provvisorio" con
    // valori qualsiasi (ma validi). Se il cliente esiste verra' sovrascritto.
    Cliente esistente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");   // placeholder valido, serve solo come contenitore
    if (!rubrica_.getById(id, esistente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string nome = leggiRiga("Nuovo nome [" + esistente.getNome() + "]: ");   // mostro il valore attuale come suggerimento
    if (nome.empty()) nome = esistente.getNome();   // riga vuota = "non voglio cambiare questo campo"

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
        rubrica_.modificaCliente(id, nome, cognome, telefono, email, cf, dataNascita);   // qui scattano di nuovo tutte le validazioni
        std::cout << "Cliente modificato con successo." << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Errore: " << e.what() << std::endl;   // nota per me: qui non rientro in un ciclo di retry come in "aggiungi cliente"
    }
}

// -----------------------------------------------------------------------------
// Elimina cliente
// -----------------------------------------------------------------------------
void Menu::eliminaClienteInterattivo() {
    mostraSchermata("ELIMINA CLIENTE", true);
    stampaListaClienti(rubrica_.visualizzaTutti());   // cosi' l'utente vede subito gli id tra cui scegliere
    std::cout << std::endl;
    std::string idStr = leggiRiga("ID cliente da eliminare: ");
    int id;
    try {
        id = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    if (rubrica_.eliminaCliente(id)) {   // eliminaCliente ritorna un bool, comodo da usare direttamente nell'if
        std::cout << "Cliente eliminato." << std::endl;
    } else {
        std::cout << "Cliente non trovato." << std::endl;
    }
}

// -----------------------------------------------------------------------------
// Cerca cliente (per nome o cognome, anche parziale)
// -----------------------------------------------------------------------------
void Menu::cercaClienteInterattivo() {
    mostraSchermata("CERCA CLIENTE", true);
    std::string nome = leggiRiga("Nome (vuoto per non filtrare): ");
    std::string cognome = leggiRiga("Cognome (vuoto per non filtrare): ");
    // I due campi sono in AND: se sono entrambi compilati, il cliente deve
    // combaciare su tutti e due. Un campo vuoto non viene usato come filtro.
    std::vector<Cliente> risultati = rubrica_.cercaPerNomeECognome(nome, cognome);

    if (risultati.empty()) {
        std::cout << "Nessun cliente trovato." << std::endl;
        return;
    }
    for (const Cliente& c : risultati) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << std::endl;
    }
}

// =============================================================================
// SOTTOMENU INTERAZIONI
// =============================================================================
void Menu::eseguiSottomenuInterazioni() {
    while (true) {
        mostraSchermata("GESTIONE INTERAZIONI");
        std::cout << "1. Aggiungi appuntamento\n"
                  << "2. Aggiungi contratto\n"
                  << "3. Visualizza interazioni di un cliente\n"
                  << "4. Cerca interazioni\n"
                  << "0. Torna al menu principale\n"
                  << "Scelta: ";

        int scelta = leggiSceltaMenu();
        try {
            switch (scelta) {
                case 1: aggiungiAppuntamentoInterattivo();         attendiInvio(); break;
                case 2: aggiungiContrattoInterattivo();            attendiInvio(); break;
                case 3: visualizzaInterazioniClienteInterattivo(); attendiInvio(); break;
                case 4: cercaInterazioniInterattivo();             attendiInvio(); break;
                case 0: return;
                default: std::cout << "Scelta non valida, riprova." << std::endl;
            }
        } catch (const OperazioneAnnullata&) {
            std::cout << "Operazione annullata." << std::endl;
            attendiInvio();
        }
    }
}

// -----------------------------------------------------------------------------
// Aggiungi appuntamento
// -----------------------------------------------------------------------------
void Menu::aggiungiAppuntamentoInterattivo() {
    mostraSchermata("AGGIUNGI APPUNTAMENTO", true);
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    // Serve il codice fiscale del cliente per collegargli l'interazione:
    // lo recuperiamo cercando il cliente in rubrica.
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");   // di nuovo un placeholder valido come contenitore
    if (!rubrica_.getById(idCliente, cliente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;   // niente cliente, niente interazione: non avrebbe senso a chi collegarla
    }

    std::string data = leggiRiga("Data (gg/mm/aaaa): ");
    std::string orario = leggiRiga("Orario (hh:mm): ");
    std::string esito = leggiRiga("Esito: ");
    std::string note = leggiRiga("Note (facoltative): ");   // possono restare vuote, Appuntamento le accetta cosi'

    // Stesso schema di "aggiungi cliente": si riprova finche' tutti i campi
    // sono validi, richiedendo di volta in volta solo quello sbagliato.
    while (true) {
        try {
            storico_.aggiungiAppuntamento(idCliente, cliente.getCodiceFiscale(),
                                          data, orario, esito, note);   // il CF del cliente lo prendo dall'oggetto appena recuperato, non lo richiedo all'utente
            std::cout << "Appuntamento aggiunto con successo." << std::endl;
            return;
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
                data = leggiRiga("Reinserisci data: ");   // fallback prudente
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Aggiungi contratto
// -----------------------------------------------------------------------------
void Menu::aggiungiContrattoInterattivo() {
    mostraSchermata("AGGIUNGI CONTRATTO", true);
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
    std::string premioStr = leggiRiga("Premio: ");           // lo tengo come stringa: la conversione a double avviene sotto, dentro il ciclo
    std::string dataScadenza = leggiRiga("Data scadenza (gg/mm/aaaa): ");

    while (true) {
        // Prima trasformiamo il premio da testo a numero: se non e' un numero
        // lo richiediamo subito e ricominciamo il ciclo.
        double premio;
        try {
            premio = std::stod(premioStr);   // conversione separata dalla creazione del Contratto: errori diversi, messaggi diversi
        } catch (const std::exception&) {
            std::cout << "Errore: Premio non numerico." << std::endl;
            premioStr = leggiRiga("Reinserisci premio: ");
            continue;   // torno subito all'inizio del while, senza nemmeno provare a creare il contratto
        }

        // Poi proviamo a creare il contratto: se un campo non e' valido,
        // richiediamo solo quello.
        try {
            storico_.aggiungiContratto(idCliente, cliente.getCodiceFiscale(),
                                       data, tipoPolizza, premio, dataScadenza);
            std::cout << "Contratto aggiunto con successo." << std::endl;
            return;
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
                data = leggiRiga("Reinserisci data: ");   // fallback prudente
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Visualizza le interazioni di un cliente
// -----------------------------------------------------------------------------
void Menu::visualizzaInterazioniClienteInterattivo() {
    mostraSchermata("INTERAZIONI CLIENTE", true);
    stampaListaClienti(rubrica_.visualizzaTutti());   // cosi' l'utente vede subito gli id tra cui scegliere
    std::cout << std::endl;
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    // Non serve che il cliente esista ancora in rubrica: le interazioni di un
    // cliente eliminato restano comunque consultabili (scelta di progetto).
    // Passiamo "" come codice fiscale per filtrare solo in base all'id.
    std::vector<const Interazione*> interazioni = storico_.getPerCliente(idCliente, "");   // "" = non filtro anche per codice fiscale
    if (interazioni.empty()) {
        std::cout << "Nessuna interazione trovata per questo cliente." << std::endl;
        return;
    }

    // Recupero nome e cognome per mostrarli al posto del solo id. Se il
    // cliente e' stato eliminato nel frattempo, lo segnalo ma mostro comunque
    // le interazioni (scelta di progetto invariata, vedi sopra).
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");   // placeholder valido, riempito solo se getById trova il cliente
    std::string descrizioneCliente = rubrica_.getById(idCliente, cliente)
        ? cliente.getNome() + " " + cliente.getCognome()
        : "cliente non trovato";

    for (const Interazione* i : interazioni) {
        i->stampa(descrizioneCliente);     // chiama la stampa giusta (Appuntamento o Contratto)
    }
}

// -----------------------------------------------------------------------------
// Cerca interazioni per data
// -----------------------------------------------------------------------------
// Se l'utente lascia il campo vuoto, vengono mostrate tutte le interazioni.
void Menu::cercaInterazioniInterattivo() {
    mostraSchermata("CERCA INTERAZIONI", true);
    std::string data = leggiRiga("Cerca per data (gg/mm/aaaa, vuoto per saltare): ");

    for (const std::unique_ptr<Interazione>& interazione : storico_.tutte()) {   // scorro l'intera lista globale, senza passare da getPerCliente
        if (data.empty() || interazione->getData() == data) {   // campo vuoto -> mostro tutto; altrimenti solo le date che combaciano esattamente
            interazione->stampa();
        }
    }
}

// =============================================================================
// USCITA
// =============================================================================
void Menu::salvaESci() {
    FileManager::salvaClienti(rubrica_);       // sovrascrive clienti.csv con lo stato attuale della rubrica
    FileManager::salvaInterazioni(storico_);   // sovrascrive appuntamenti.csv e contratti.csv
    std::cout << "Dati salvati. Uscita in corso." << std::endl;
}
