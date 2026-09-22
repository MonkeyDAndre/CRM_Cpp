#include "Appuntamento.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>   // std::invalid_argument

// -----------------------------------------------------------------------------
// Costruttore
// -----------------------------------------------------------------------------
// La lista di inizializzazione ": Interazione(...)" costruisce prima la parte
// comune (che valida la data). Poi qui controlliamo orario ed esito: devono
// essere non vuoti. Le note invece sono libere e possono restare vuote.
Appuntamento::Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                           const std::string& data, const std::string& orario,
                           const std::string& esito, const std::string& note)
    : Interazione(id, idCliente, codiceFiscaleCliente, data), note_(note) {   // note_ non ha bisogno di validazione, la assegno subito qui
    if (!Validatore::campoNonVuoto(orario)) {   // orario obbligatorio
        throw std::invalid_argument("Orario non valido: non puo' essere vuoto");
    }
    if (!Validatore::campoNonVuoto(esito)) {   // esito obbligatorio
        throw std::invalid_argument("Esito non valido: non puo' essere vuoto");
    }
    orario_ = orario;   // arrivo qui solo se entrambi i controlli sono passati
    esito_ = esito;
}

std::string Appuntamento::getOrario() const { return orario_; }   // lettura semplice
std::string Appuntamento::getEsito() const  { return esito_; }    // lettura semplice
std::string Appuntamento::getNote() const   { return note_; }     // lettura semplice

// -----------------------------------------------------------------------------
// toCSV: costruisce la riga da salvare in appuntamenti.csv
// -----------------------------------------------------------------------------
// Ordine delle colonne: id,id_cliente,codice_fiscale,data,orario,esito,note
// Solo le note passano da escapeCampoCSV perche' sono l'unico campo che puo'
// contenere virgole.
std::string Appuntamento::toCSV() const {
    return std::to_string(getId()) + "," +          // colonna 1: id
           std::to_string(getIdCliente()) + "," +   // colonna 2: id del cliente collegato
           getCodiceFiscaleCliente() + "," +         // colonna 3: codice fiscale duplicato
           getData() + "," +                         // colonna 4: data dell'appuntamento
           orario_ + "," +                           // colonna 5: orario
           esito_ + "," +                             // colonna 6: esito
           escapeCampoCSV(note_);                     // colonna 7: note, protette se contengono virgole/virgolette
}

// -----------------------------------------------------------------------------
// stampa: mostra l'appuntamento a video in forma leggibile
// -----------------------------------------------------------------------------
// Se descrizioneCliente e' vuota, mostra "Cliente <id>" come prima. Se viene
// passata (nome e cognome, o "cliente non trovato"), mostra "id: <id> - ..."
// al suo posto: e' il chiamante a decidere se ha il contesto per farlo.
void Appuntamento::stampa(const std::string& descrizioneCliente) const {
    std::cout << "[Appuntamento #" << getId() << "] ";
    if (descrizioneCliente.empty()) {
        std::cout << "Cliente " << getIdCliente();
    } else {
        std::cout << "id: " << getIdCliente() << " - " << descrizioneCliente;
    }
    std::cout << " - " << getData() << " " << orario_ << " - " << esito_;   // riga principale con i dati essenziali
    if (!note_.empty()) {           // le note le mostro solo se ci sono, altrimenti la riga resta piu' pulita
        std::cout << " (" << note_ << ")";
    }
    std::cout << std::endl;   // vado a capo e forzo il flush del buffer di output
}

// -----------------------------------------------------------------------------
// getTipo: etichetta testuale del tipo, utile per filtri e salvataggio
// -----------------------------------------------------------------------------
std::string Appuntamento::getTipo() const {
    return "Appuntamento";   // stringa fissa: usata da StoricoInterazioni/FileManager per riconoscere il tipo senza RTTI
}
