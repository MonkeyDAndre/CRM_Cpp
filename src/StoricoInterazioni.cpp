#include "StoricoInterazioni.h"
#include "Appuntamento.h"
#include "Contratto.h"
#include <utility>   

// -----------------------------------------------------------------------------
// aggiungiAppuntamento
// -----------------------------------------------------------------------------
int StoricoInterazioni::aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                                             const std::string& data, const std::string& orario,
                                             const std::string& esito, const std::string& note) {

    std::unique_ptr<Interazione> nuovo = std::unique_ptr<Interazione>(new Appuntamento(prossimoId_, idCliente, codiceFiscaleCliente,
                         data, orario, esito, note));   // il puntatore e' dichiarato come Interazione* anche se punta a un Appuntamento: e' polimorfismo
           // se il costruttore di Appuntamento lancia una eccezione, "nuovo" non viene mai creato

    // push_back sposta il puntatore dentro il vector (std::move e' automatico
    // per un temporaneo). Da qui in poi la lista "possiede" l'oggetto.
    interazioni_.push_back(std::move(nuovo));   // std::move esplicito perche' "nuovo" e' una variabile con nome, non un temporaneo

    int idAssegnato = prossimoId_;   // salvo l'id appena usato prima di incrementare il contatore
    prossimoId_++;
    return idAssegnato;
}

// -----------------------------------------------------------------------------
// aggiungiContratto (stessa logica di aggiungiAppuntamento)
// -----------------------------------------------------------------------------
int StoricoInterazioni::aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                                          const std::string& data, const std::string& tipoPolizza,
                                          double premio, const std::string& dataScadenza) {
    std::unique_ptr<Interazione> nuovo = std::unique_ptr<Interazione>(new Contratto(prossimoId_, idCliente, codiceFiscaleCliente,
                      data, tipoPolizza, premio, dataScadenza));   // stesso schema di sopra, ma creo un Contratto

    interazioni_.push_back(std::move(nuovo));   // la lista diventa proprietaria anche di questo oggetto

    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

// -----------------------------------------------------------------------------
// getPerCliente: filtra la lista globale
// -----------------------------------------------------------------------------
std::vector<const Interazione*> StoricoInterazioni::getPerCliente(
    int idCliente, const std::string& codiceFiscale) const {
    std::vector<const Interazione*> risultati;   // qui non copio le interazioni, salvo solo puntatori a quelle esistenti

    for (const std::unique_ptr<Interazione>& interazione : interazioni_) {   // scorro tutta la lista globale, elemento per elemento
        bool stessoId = interazione->getIdCliente() == idCliente;   // primo criterio: l'id del cliente deve combaciare

        // Se il codice fiscale richiesto e' vuoto, non lo consideriamo un
        // filtro; altrimenti deve combaciare con quello salvato.
        bool stessoCodiceFiscale = codiceFiscale.empty() || interazione->getCodiceFiscaleCliente() == codiceFiscale; 

        if (stessoId && stessoCodiceFiscale) {   // devono valere entrambe le condizioni
            // .get() estrae il puntatore "grezzo" dall'unique_ptr
            risultati.push_back(interazione.get());   // aggiungo il puntatore grezzo, non l'unique_ptr (che non si puo' copiare)
        }
    }
    return risultati;
}

// -----------------------------------------------------------------------------
// eliminaInterazione
// -----------------------------------------------------------------------------
bool StoricoInterazioni::eliminaInterazione(int id) {
    for (size_t i = 0; i < interazioni_.size(); ++i) {   // ricerca lineare come nella Rubrica
        if (interazioni_[i]->getId() == id) {            // -> perche' l'elemento e' un unique_ptr, uso -> per arrivare all'oggetto puntato
            interazioni_.erase(interazioni_.begin() + i);   // erase su un unique_ptr distrugge anche l'oggetto puntato (libera la memoria)
            return true;
        }
    }
    return false;   // nessuna interazione con quell'id
}

// -----------------------------------------------------------------------------
// Contatore di id e accesso alla lista
// -----------------------------------------------------------------------------
void StoricoInterazioni::impostaProssimoId(int valore) {
    prossimoId_ = valore;   // usato dal FileManager per allineare il contatore condiviso agli id letti dai due CSV
}

int StoricoInterazioni::getProssimoId() const {
    return prossimoId_;   // lettura semplice
}

const std::vector<std::unique_ptr<Interazione>>& StoricoInterazioni::tutte() const {
    return interazioni_;   // riferimento diretto alla lista interna, in sola lettura per chi la riceve
}
