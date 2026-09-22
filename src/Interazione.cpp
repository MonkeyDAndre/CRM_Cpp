#include "Interazione.h"
#include "Validatore.h"
#include <stdexcept>   

// -----------------------------------------------------------------------------
// Costruttore della parte comune
// -----------------------------------------------------------------------------
// id, idCliente e codice fiscale vengono salvati cosi' come arrivano; la data
// invece viene controllata: se non e' nel formato gg/mm/aaaa si lancia
// un'eccezione e l'interazione non viene creata.
Interazione::Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente,
                         const std::string& data)
    : id_(id), idCliente_(idCliente), codiceFiscaleCliente_(codiceFiscaleCliente) { 
    // questi tre si assegnano direttamente, nessun controllo su di loro qui 
    //perchè già sono stati validati durante la creazione del cliente
    if (!Validatore::dataValida(data)) {   // la data invece la voglio verificata prima di salvarla
        throw std::invalid_argument("Data non valida: '" + data + "' (formato atteso gg/mm/aaaa)");
    }
    data_ = data;   // arrivo qui solo se la data e' valida
}

// -----------------------------------------------------------------------------
// Getter
// -----------------------------------------------------------------------------
int Interazione::getId() const                          { return id_; }                     // lettura semplice
int Interazione::getIdCliente() const                   { return idCliente_; }               // lettura semplice
std::string Interazione::getCodiceFiscaleCliente() const { return codiceFiscaleCliente_; }    // lettura semplice
std::string Interazione::getData() const                 { return data_; }                    // lettura semplice

// -----------------------------------------------------------------------------
// escapeCampoCSV: rende sicuro un campo di testo dentro un CSV
// -----------------------------------------------------------------------------
// Regola standard del formato CSV:
//  - se il testo contiene una virgola o una virgoletta, va racchiuso fra " ";
//  - ogni virgoletta interna va raddoppiata ("" al posto di ").
// Se il testo non ha caratteri "pericolosi" lo si lascia identico.
std::string Interazione::escapeCampoCSV(const std::string& campo) {
    bool serveEscape = campo.find(',') != std::string::npos ||   // c'e' almeno una virgola nel testo?
                       campo.find('"') != std::string::npos;     // oppure almeno una virgoletta?

    if (!serveEscape) {
        return campo;   // niente di pericoloso: restituisco il campo cosi' com'e', senza toccarlo
    }

    std::string risultato = "\"";   // apro le virgolette del campo quotato
    for (char c : campo) {          // scorro ogni carattere del testo originale
        if (c == '"') {
            risultato += "\"\"";   // virgoletta raddoppiata
        } else {
            risultato += c;        // carattere normale, lo copio cosi' com'e'
        }
    }
    risultato += "\"";   // chiudo le virgolette del campo quotato
    return risultato;
}
