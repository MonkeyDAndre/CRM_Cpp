#ifndef RUBRICA_H
#define RUBRICA_H

#include <vector>   
#include <string>
#include "Cliente.h"

// -----------------------------------------------------------------------------
// Rubrica
// -----------------------------------------------------------------------------
// Contiene TUTTI i clienti del CRM dentro un std::vector<Cliente> e offre le
// operazioni CRUD:
//   Create  -> aggiungiCliente
//   Read    -> visualizzaTutti / getById / cercaPerNomeCognome
//   Update  -> modificaCliente
//   Delete  -> eliminaCliente
//
// Ogni cliente ha un id numerico progressivo. "prossimoId_" tiene in memoria
// quale sara' l'id del prossimo cliente inserito. All'avvio del programma il
// FileManager lo aggiorna in base agli id gia' presenti nel file.
// -----------------------------------------------------------------------------
class Rubrica {
public:
    // Crea un nuovo cliente con id automatico e lo aggiunge alla lista.
    // Ritorna l'id assegnato. Se un campo non e' valido, il costruttore di
    // Cliente lancia std::invalid_argument e nulla viene aggiunto.
    int aggiungiCliente(const std::string& nome, const std::string& cognome,
                        const std::string& telefono, const std::string& email,
                        const std::string& codiceFiscale, const std::string& dataNascita);

    // Restituisce (in sola lettura) la lista completa dei clienti.
    const std::vector<Cliente>& visualizzaTutti() const;   // riferimento const: niente copie inutili di tutto il vector

    // Sostituisce i dati del cliente con quell'id. Ritorna false se l'id non
    // esiste. Se i nuovi dati non sono validi lancia std::invalid_argument e il
    // cliente NON viene toccato.
    bool modificaCliente(int id, const std::string& nome, const std::string& cognome,
                         const std::string& telefono, const std::string& email,
                         const std::string& codiceFiscale, const std::string& dataNascita);

    // Rimuove il cliente con quell'id. Ritorna false se l'id non esiste.
    // Nota: le interazioni collegate NON vengono toccate (scelta di progetto).
    bool eliminaCliente(int id);

    // Cerca il cliente con quell'id: se lo trova lo copia in "risultato" e
    // ritorna true; altrimenti ritorna false e "risultato" resta com'era.
    bool getById(int id, Cliente& risultato) const;   // "risultato" e' un parametro di output passato per riferimento

    // Ritorna tutti i clienti il cui nome O cognome contiene "query"
    // (confronto senza distinzione fra maiuscole e minuscole).
    std::vector<Cliente> cercaPerNomeCognome(const std::string& query) const;   // qui ritorno per valore: e' un elenco nuovo, non un riferimento a clienti_

    // Ritorna i clienti che combaciano su ENTRAMBI i campi forniti (parziale,
    // case-insensitive). Un campo lasciato vuoto non viene usato come filtro:
    // se entrambi sono vuoti, ritorna tutti i clienti.
    std::vector<Cliente> cercaPerNomeECognome(const std::string& nome, const std::string& cognome) const;

    // Gestione del contatore di id (usati dal FileManager al caricamento).
    void impostaProssimoId(int valore);
    int getProssimoId() const;

private:
    std::vector<Cliente> clienti_;   // la collezione vera e propria: tutti i clienti vivono qui dentro
    int prossimoId_ = 1;             // valore iniziale di default: il primo cliente creato da zero avra' id 1

    // Ritorna la posizione nel vector del cliente con quell'id, oppure -1.
    int trovaIndicePerId(int id) const;   // helper privato, usato da modifica/elimina/getById per non ripetere la ricerca

    // Ritorna una copia della stringa con tutte le lettere minuscole.
    static std::string minuscolo(const std::string& s);   // static: non serve un oggetto Rubrica per chiamarla
};

#endif   // RUBRICA_H
