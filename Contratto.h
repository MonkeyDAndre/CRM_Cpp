#ifndef CONTRATTO_H
#define CONTRATTO_H

#include "Interazione.h"

class Contratto : public Interazione {
public:
    Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
              const std::string& data, const std::string& tipoPolizza,
              double premio, const std::string& dataScadenza);

    std::string getTipoPolizza() const;
    double getPremio() const;
    std::string getDataScadenza() const;

    std::string toCSV() const override;
    void stampa() const override;
    std::string getTipo() const override;

private:
    std::string tipoPolizza_;
    double premio_;
    std::string dataScadenza_;
};

#endif