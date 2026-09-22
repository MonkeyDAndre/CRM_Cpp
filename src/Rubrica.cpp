#include "Rubrica.h"
#include <cctype>   // std::tolower, lavora su un singolo char

// -----------------------------------------------------------------------------
// minuscolo: copia la stringa mettendo tutte le lettere in minuscolo
// -----------------------------------------------------------------------------
// Serve per la ricerca "case-insensitive": confrontiamo sempre versioni tutte
// minuscole, cosi' "rossi", "Rossi" e "ROSSI" risultano uguali.
std::string Rubrica::minuscolo(const std::string& s) {
    std::string risultato = s;   // parto da una copia, non tocco la stringa originale (che e' const&)
    for (size_t i = 0; i < risultato.size(); ++i) {   // scorro ogni carattere per indice
        risultato[i] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(risultato[i])));   // converto in minuscolo, col cast a unsigned char per sicurezza
    }
    return risultato;   // ritorno la copia trasformata
}

// -----------------------------------------------------------------------------
// trovaIndicePerId: scorre il vector e cerca il cliente con quell'id
// -----------------------------------------------------------------------------
// Ritorna l'indice (0, 1, 2, ...) se lo trova, altrimenti -1.
int Rubrica::trovaIndicePerId(int id) const {
    for (size_t i = 0; i < clienti_.size(); ++i) {   // ricerca lineare: per un progetto di queste dimensioni va benissimo
        if (clienti_[i].getId() == id) {             // confronto l'id del cliente in posizione i con quello cercato
            return static_cast<int>(i);              // trovato: ritorno la posizione (cast perche' size() e' size_t, non int)
        }
    }
    return -1;   // scorsa tutta la lista senza trovarlo: nessun cliente con quell'id
}

// -----------------------------------------------------------------------------
// aggiungiCliente
// -----------------------------------------------------------------------------
int Rubrica::aggiungiCliente(const std::string& nome, const std::string& cognome,
                             const std::string& telefono, const std::string& email,
                             const std::string& codiceFiscale, const std::string& dataNascita) {
    // Costruisco il nuovo cliente usando l'id corrente del contatore.
    // Se un campo non e' valido, questa riga lancia e la funzione termina qui.
    Cliente nuovo(prossimoId_, nome, cognome, telefono, email, codiceFiscale, dataNascita);   // se il costruttore lancia, non arrivo alla riga sotto

    // Aggiungo il cliente in fondo alla lista.
    clienti_.push_back(nuovo);   // push_back copia "nuovo" dentro il vector

    // Il prossimo cliente avra' l'id successivo. Ritorno quello appena usato.
    int idAssegnato = prossimoId_;   // salvo il valore PRIMA di incrementarlo, e' quello che ho appena usato
    prossimoId_++;                   // preparo il contatore per la prossima chiamata
    return idAssegnato;
}

// -----------------------------------------------------------------------------
// visualizzaTutti
// -----------------------------------------------------------------------------
const std::vector<Cliente>& Rubrica::visualizzaTutti() const {
    return clienti_;   // ritorno un riferimento diretto: chi chiama vede sempre lo stato attuale, senza copie
}

// -----------------------------------------------------------------------------
// modificaCliente
// -----------------------------------------------------------------------------
bool Rubrica::modificaCliente(int id, const std::string& nome, const std::string& cognome,
                              const std::string& telefono, const std::string& email,
                              const std::string& codiceFiscale, const std::string& dataNascita) {
    int indice = trovaIndicePerId(id);   // prima di tutto: esiste un cliente con questo id?
    if (indice == -1) {
        return false;   // nessun cliente con quell'id
    }

    // Creo prima un cliente "aggiornato" a parte: se qualche campo non e'
    // valido, la riga qui sotto lancia PRIMA di toccare la lista, quindi il
    // cliente originale resta intatto.
    Cliente aggiornato(id, nome, cognome, telefono, email, codiceFiscale, dataNascita);   // se qualcosa non va, l'eccezione parte da qui

    // Sostituisco il vecchio cliente con quello aggiornato.
    clienti_[indice] = aggiornato;   // operatore di assegnazione tra Cliente: copia tutti i campi
    return true;
}

// -----------------------------------------------------------------------------
// eliminaCliente
// -----------------------------------------------------------------------------
bool Rubrica::eliminaCliente(int id) {
    int indice = trovaIndicePerId(id);   // cerco la posizione del cliente da togliere
    if (indice == -1) {
        return false;   // niente da eliminare, nessun errore, solo "non trovato"
    }
    // erase toglie l'elemento in quella posizione e compatta il vector.
    clienti_.erase(clienti_.begin() + indice);   // begin() + indice e' un iteratore che punta esattamente a quell'elemento
    return true;
}

// -----------------------------------------------------------------------------
// getById
// -----------------------------------------------------------------------------
bool Rubrica::getById(int id, Cliente& risultato) const {
    int indice = trovaIndicePerId(id);
    if (indice == -1) {
        return false;   // "risultato" non viene toccato: resta quello che aveva passato il chiamante
    }
    risultato = clienti_[indice];   // copio il cliente trovato nel parametro
    return true;
}

// -----------------------------------------------------------------------------
// cercaPerNomeCognome
// -----------------------------------------------------------------------------
std::vector<Cliente> Rubrica::cercaPerNomeCognome(const std::string& query) const {
    std::vector<Cliente> risultati;   // qui accumulo i clienti che soddisfano la ricerca

    // Preparo una volta sola la versione minuscola di cio' che sto cercando.
    std::string queryMinuscola = minuscolo(query);   // la calcolo fuori dal ciclo, non a ogni iterazione

    // Per ogni cliente controllo se la query compare nel nome o nel cognome.
    for (const Cliente& cliente : clienti_) {   // range-based for: scorro tutti i clienti senza gestire indici a mano
        std::string nomeMin = minuscolo(cliente.getNome());       // nome del cliente corrente, in minuscolo
        std::string cognomeMin = minuscolo(cliente.getCognome()); // idem per il cognome

        // std::string::find ritorna npos se la sottostringa non c'e'.
        bool trovatoNelNome = nomeMin.find(queryMinuscola) != std::string::npos;       // true se la query compare da qualche parte nel nome
        bool trovatoNelCognome = cognomeMin.find(queryMinuscola) != std::string::npos; // true se compare nel cognome

        if (trovatoNelNome || trovatoNelCognome) {   // basta un match in uno dei due campi
            risultati.push_back(cliente);             // lo aggiungo (per copia) all'elenco dei risultati
        }
    }
    return risultati;   // puo' essere vuoto, se nessuno combacia
}

// -----------------------------------------------------------------------------
// cercaPerNomeECognome
// -----------------------------------------------------------------------------
// A differenza di cercaPerNomeCognome (un'unica query su nome O cognome), qui
// i due campi sono indipendenti e vanno ENTRAMBI in AND: un cliente combacia
// solo se soddisfa ogni campo che e' stato effettivamente fornito. Un campo
// lasciato vuoto non filtra nulla (il suo "trovatoOk" e' sempre true).
std::vector<Cliente> Rubrica::cercaPerNomeECognome(const std::string& nome, const std::string& cognome) const {
    std::vector<Cliente> risultati;

    std::string nomeMinuscolo = minuscolo(nome);
    std::string cognomeMinuscolo = minuscolo(cognome);

    for (const Cliente& cliente : clienti_) {
        bool nomeOk = nome.empty() ||
                      minuscolo(cliente.getNome()).find(nomeMinuscolo) != std::string::npos;
        bool cognomeOk = cognome.empty() ||
                         minuscolo(cliente.getCognome()).find(cognomeMinuscolo) != std::string::npos;

        if (nomeOk && cognomeOk) {   // entrambi i filtri (quelli attivi) devono valere
            risultati.push_back(cliente);
        }
    }
    return risultati;
}

// -----------------------------------------------------------------------------
// Contatore di id
// -----------------------------------------------------------------------------
void Rubrica::impostaProssimoId(int valore) {
    prossimoId_ = valore;   // usato dal FileManager per allineare il contatore agli id gia' letti dal CSV
}

int Rubrica::getProssimoId() const {
    return prossimoId_;   // semplice lettura, nessuna logica
}
