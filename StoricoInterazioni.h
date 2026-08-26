#ifndef STORICOINTERAZIONI_H
#define STORICOINTERAZIONI_H

#include <vector>
#include <memory>
#include <string>
#include "Interazione.h"

class StoricoInterazioni {
public:
    int aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                              const std::string& data, const std::string& orario,
                              const std::string& esito, const std::string& note);

    int aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                           const std::string& data, const std::string& tipoPolizza,
                           double premio, const std::string& dataScadenza);

    std::vector<const Interazione*> getPerCliente(int idCliente,
                                                   const std::string& codiceFiscale) const;

    bool eliminaInterazione(int id);

    void impostaProssimoId(int valore);
    int getProssimoId() const;

    const std::vector<std::unique_ptr<Interazione>>& tutte() const;

private:
    std::vector<std::unique_ptr<Interazione>> interazioni_;
    int prossimoId_ = 1;
};

#endif