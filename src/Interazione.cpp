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
