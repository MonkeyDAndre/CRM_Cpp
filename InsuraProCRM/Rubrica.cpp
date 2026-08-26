#include "Rubrica.h"
#include <algorithm>
#include <cctype>

std::string Rubrica::minuscolo(const std::string& s) {
    std::string risultato = s;
    std::transform(risultato.begin(), risultato.end(), risultato.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return risultato;
}

int Rubrica::trovaIndicePerId(int id) const {
    for (size_t i = 0; i < clienti_.size(); ++i) {
        if (clienti_[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

int Rubrica::aggiungiCliente(const std::string& nome, const std::string& cognome,
                              const std::string& telefono, const std::string& email,
                              const std::string& codiceFiscale, const std::string& dataNascita) {
    Cliente nuovo(prossimoId_, nome, cognome, telefono, email, codiceFiscale, dataNascita);
    clienti_.push_back(nuovo);
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

const std::vector<Cliente>& Rubrica::visualizzaTutti() const {
    return clienti_;
}

bool Rubrica::modificaCliente(int id, const std::string& nome, const std::string& cognome,
                               const std::string& telefono, const std::string& email,
                               const std::string& codiceFiscale, const std::string& dataNascita) {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;

    // Costruisco un Cliente temporaneo: se la validazione fallisce, lancia PRIMA
    // di toccare clienti_[indice], garantendo nessuna scrittura parziale.
    Cliente aggiornato(id, nome, cognome, telefono, email, codiceFiscale, dataNascita);
    clienti_[indice] = aggiornato;
    return true;
}

bool Rubrica::eliminaCliente(int id) {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;
    clienti_.erase(clienti_.begin() + indice);
    return true;
}

bool Rubrica::getById(int id, Cliente& risultato) const {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;
    risultato = clienti_[indice];
    return true;
}

std::vector<Cliente> Rubrica::cercaPerNomeCognome(const std::string& query) const {
    std::vector<Cliente> risultati;
    std::string queryMinuscola = minuscolo(query);
    for (const auto& cliente : clienti_) {
        std::string nomeMinuscolo = minuscolo(cliente.getNome());
        std::string cognomeMinuscolo = minuscolo(cliente.getCognome());
        if (nomeMinuscolo.find(queryMinuscola) != std::string::npos ||
            cognomeMinuscolo.find(queryMinuscola) != std::string::npos) {
            risultati.push_back(cliente);
        }
    }
    return risultati;
}

void Rubrica::impostaProssimoId(int valore) {
    prossimoId_ = valore;
}

int Rubrica::getProssimoId() const {
    return prossimoId_;
}