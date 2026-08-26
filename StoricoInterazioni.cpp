#include "StoricoInterazioni.h"
#include "Appuntamento.h"
#include "Contratto.h"

int StoricoInterazioni::aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                                              const std::string& data, const std::string& orario,
                                              const std::string& esito, const std::string& note) {
    interazioni_.push_back(std::unique_ptr<Interazione>(
        new Appuntamento(prossimoId_, idCliente, codiceFiscaleCliente, data, orario, esito, note)));
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

int StoricoInterazioni::aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                                           const std::string& data, const std::string& tipoPolizza,
                                           double premio, const std::string& dataScadenza) {
    interazioni_.push_back(std::unique_ptr<Interazione>(
        new Contratto(prossimoId_, idCliente, codiceFiscaleCliente, data, tipoPolizza, premio,
                       dataScadenza)));
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

std::vector<const Interazione*> StoricoInterazioni::getPerCliente(
    int idCliente, const std::string& codiceFiscale) const {
    std::vector<const Interazione*> risultati;
    for (const auto& interazione : interazioni_) {
        bool matchId = interazione->getIdCliente() == idCliente;
        bool matchCf = codiceFiscale.empty() ||
                       interazione->getCodiceFiscaleCliente() == codiceFiscale;
        if (matchId && matchCf) {
            risultati.push_back(interazione.get());
        }
    }
    return risultati;
}

bool StoricoInterazioni::eliminaInterazione(int id) {
    for (size_t i = 0; i < interazioni_.size(); ++i) {
        if (interazioni_[i]->getId() == id) {
            interazioni_.erase(interazioni_.begin() + i);
            return true;
        }
    }
    return false;
}

void StoricoInterazioni::impostaProssimoId(int valore) {
    prossimoId_ = valore;
}

int StoricoInterazioni::getProssimoId() const {
    return prossimoId_;
}

const std::vector<std::unique_ptr<Interazione>>& StoricoInterazioni::tutte() const {
    return interazioni_;
}