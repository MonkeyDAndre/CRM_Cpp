#include "Cliente.h"
#include "Validatore.h"
#include <stdexcept>   // std::invalid_argument

// -----------------------------------------------------------------------------
// Costruttore
// -----------------------------------------------------------------------------
// Salva l'id nella lista di inizializzazione (":") e poi chiama i setter per
// tutti gli altri campi. In questo modo:
//  - la validazione e' scritta una volta sola (dentro i setter);
//  - se un valore non e' valido, il setter lancia un'eccezione e la creazione
//    dell'oggetto fallisce subito.
Cliente::Cliente(int id, const std::string& nome, const std::string& cognome,
                 const std::string& telefono, const std::string& email,
                 const std::string& codiceFiscale, const std::string& dataNascita)
    : id_(id) {   // solo l'id viene assegnato direttamente qui, gli altri campi passano dai setter nel corpo
    setNome(nome);                    // se qui dentro parte un'eccezione, i campi successivi non vengono nemmeno provati
    setCognome(cognome);
    setTelefono(telefono);
    setEmail(email);
    setCodiceFiscale(codiceFiscale);
    setDataNascita(dataNascita);
}

// -----------------------------------------------------------------------------
// Getter (una riga ciascuno: restituiscono il valore del campo)
// -----------------------------------------------------------------------------
int Cliente::getId() const                  { return id_; }             // semplice lettura, nessuna logica
std::string Cliente::getNome() const         { return nome_; }          // idem
std::string Cliente::getCognome() const      { return cognome_; }       // idem
std::string Cliente::getTelefono() const      { return telefono_; }     // idem
std::string Cliente::getEmail() const         { return email_; }        // idem
std::string Cliente::getCodiceFiscale() const { return codiceFiscale_; } // idem
std::string Cliente::getDataNascita() const   { return dataNascita_; }  // idem

// -----------------------------------------------------------------------------
// Setter
// -----------------------------------------------------------------------------
// Ogni setter segue sempre lo stesso schema:
//  1. chiedo a Validatore se il valore va bene;
//  2. se NON va bene, lancio std::invalid_argument con un messaggio chiaro
//     (il campo resta invariato perche' l'assegnazione avviene solo dopo);
//  3. se va bene, salvo il valore.

void Cliente::setNome(const std::string& nome) {
    if (!Validatore::campoNonVuoto(nome)) {   // chiedo al Validatore se e' un campo di testo "vero" (non solo spazi)
        throw std::invalid_argument("Nome non valido: non puo' essere vuoto");   // lancio qui, PRIMA di toccare nome_
    }
    nome_ = nome;   // arrivo qui solo se il controllo sopra e' passato: assegnazione sicura
}

void Cliente::setCognome(const std::string& cognome) {
    if (!Validatore::campoNonVuoto(cognome)) {
        throw std::invalid_argument("Cognome non valido: non puo' essere vuoto");
    }
    cognome_ = cognome;
}

void Cliente::setTelefono(const std::string& telefono) {
    if (!Validatore::campoNonVuoto(telefono)) {
        throw std::invalid_argument("Telefono non valido: non puo' essere vuoto");
    }
    telefono_ = telefono;   // nota per me: non controllo che sia un numero di telefono "vero", solo che non sia vuoto
}

void Cliente::setEmail(const std::string& email) {
    if (!Validatore::emailValida(email)) {   // qui il controllo e' piu' fine (vedi Validatore::emailValida)
        throw std::invalid_argument("Email non valida: '" + email + "'");   // metto il valore ricevuto nel messaggio, utile per il Menu
    }
    email_ = email;
}

void Cliente::setCodiceFiscale(const std::string& codiceFiscale) {
    if (!Validatore::codiceFiscaleValido(codiceFiscale)) {
        throw std::invalid_argument(
            "Codice fiscale non valido: deve essere alfanumerico di 16 caratteri, ricevuto '" +
            codiceFiscale + "'");   // messaggio lungo apposta: spiega sia la regola sia cosa ho ricevuto
    }
    codiceFiscale_ = codiceFiscale;
}

void Cliente::setDataNascita(const std::string& dataNascita) {
    if (!Validatore::dataValida(dataNascita)) {   // controlla sia il formato sia che la data esista davvero (bisestili ecc.)
        throw std::invalid_argument(
            "Data di nascita non valida: '" + dataNascita + "' (formato atteso gg/mm/aaaa)");
    }
    dataNascita_ = dataNascita;
}
