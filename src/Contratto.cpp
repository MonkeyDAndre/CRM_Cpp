#include "Contratto.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>   // std::invalid_argument

// -----------------------------------------------------------------------------
// Costruttore
// -----------------------------------------------------------------------------
// Prima si costruisce la parte comune (che valida la data), poi si controllano
// i campi propri del contratto:
//  - tipoPolizza deve essere non vuoto;
//  - premio deve essere un numero non negativo;
//  - dataScadenza deve essere nel formato gg/mm/aaaa.
// Se un controllo fallisce si lancia un'eccezione e il contratto non nasce.
Contratto::Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
                     const std::string& data, const std::string& tipoPolizza,
                     double premio, const std::string& dataScadenza)
    : Interazione(id, idCliente, codiceFiscaleCliente, data) {   // qui la data comune viene gia' validata dal costruttore base
    if (!Validatore::campoNonVuoto(tipoPolizza)) {   // tipo polizza obbligatorio
        throw std::invalid_argument("Tipo polizza non valido: non puo' essere vuoto");
    }
    if (!Validatore::numeroPositivo(premio)) {   // premio deve essere >= 0 (vedi nota in Validatore.cpp)
        throw std::invalid_argument("Premio non valido: deve essere un numero positivo, ricevuto " +
                                    std::to_string(premio));
    }
    if (!Validatore::dataValida(dataScadenza)) {   // anche la scadenza deve essere una data reale
        throw std::invalid_argument("Data di scadenza non valida: '" + dataScadenza + "'");
    }
    tipoPolizza_ = tipoPolizza;   // arrivo qui solo se tutti e tre i controlli sopra sono passati
    premio_ = premio;
    dataScadenza_ = dataScadenza;
}

std::string Contratto::getTipoPolizza() const  { return tipoPolizza_; }    // lettura semplice
double Contratto::getPremio() const            { return premio_; }         // lettura semplice
std::string Contratto::getDataScadenza() const { return dataScadenza_; }   // lettura semplice

// -----------------------------------------------------------------------------
// toCSV: costruisce la riga da salvare in contratti.csv
// -----------------------------------------------------------------------------
// Ordine delle colonne:
//   id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza
std::string Contratto::toCSV() const {
    return std::to_string(getId()) + "," +          // colonna 1: id
           std::to_string(getIdCliente()) + "," +   // colonna 2: id del cliente collegato
           getCodiceFiscaleCliente() + "," +         // colonna 3: codice fiscale duplicato
           getData() + "," +                         // colonna 4: data di stipula
           tipoPolizza_ + "," +                       // colonna 5: tipo polizza
           std::to_string(premio_) + "," +            // colonna 6: premio (std::to_string su un double da' 6 decimali fissi)
           dataScadenza_;                              // colonna 7: data di scadenza
}

// -----------------------------------------------------------------------------
// stampa: mostra il contratto a video in forma leggibile
// -----------------------------------------------------------------------------
// Stessa logica di Appuntamento::stampa: senza descrizioneCliente mostra
// "Cliente <id>", con descrizioneCliente mostra "id: <id> - ..." al suo posto.
void Contratto::stampa(const std::string& descrizioneCliente) const {
    std::cout << "[Contratto #" << getId() << "] ";
    if (descrizioneCliente.empty()) {
        std::cout << "Cliente " << getIdCliente();
    } else {
        std::cout << "id: " << getIdCliente() << " - " << descrizioneCliente;
    }
    std::cout << " - " << tipoPolizza_ << " - premio " << premio_
              << " - scadenza " << dataScadenza_ << std::endl;   // tutta l'informazione utile su un'unica riga
}

// -----------------------------------------------------------------------------
// getTipo: etichetta testuale del tipo
// -----------------------------------------------------------------------------
std::string Contratto::getTipo() const {
    return "Contratto";   // stringa fissa, usata per smistare le interazioni nel file giusto
}
