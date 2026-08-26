#ifndef RUBRICA_H
#define RUBRICA_H

#include <vector>
#include <string>
#include "Cliente.h"

class Rubrica {
public:
    int aggiungiCliente(const std::string& nome, const std::string& cognome,
                         const std::string& telefono, const std::string& email,
                         const std::string& codiceFiscale, const std::string& dataNascita);

    const std::vector<Cliente>& visualizzaTutti() const;

    bool modificaCliente(int id, const std::string& nome, const std::string& cognome,
                          const std::string& telefono, const std::string& email,
                          const std::string& codiceFiscale, const std::string& dataNascita);

    bool eliminaCliente(int id);

    bool getById(int id, Cliente& risultato) const;

    std::vector<Cliente> cercaPerNomeCognome(const std::string& query) const;

    void impostaProssimoId(int valore);
    int getProssimoId() const;

private:
    std::vector<Cliente> clienti_;
    int prossimoId_ = 1;

    int trovaIndicePerId(int id) const; // ritorna -1 se non trovato
    static std::string minuscolo(const std::string& s);
};

#endif