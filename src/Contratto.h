#ifndef CONTRATTO_H
#define CONTRATTO_H

#include "Interazione.h"

// -----------------------------------------------------------------------------
// Contratto : public Interazione
// -----------------------------------------------------------------------------
// Interazione concreta: una polizza stipulata con il cliente.
// Oltre ai dati comuni (id, idCliente, codice fiscale, data) aggiunge:
//   - tipoPolizza   (es. "Auto", "Casa", "Vita")
//   - premio        (importo in euro, numero non negativo)
//   - dataScadenza  (formato gg/mm/aaaa)
// -----------------------------------------------------------------------------
class Contratto : public Interazione {
public:
    Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
              const std::string& data, const std::string& tipoPolizza,
              double premio, const std::string& dataScadenza);

    std::string getTipoPolizza() const;   // getter dei soli campi aggiunti da questa classe
    double getPremio() const;
    std::string getDataScadenza() const;

    // Versioni specifiche per il Contratto dei metodi della classe base.
    std::string toCSV() const override;
    void stampa(const std::string& descrizioneCliente = "") const override;
    std::string getTipo() const override;   // ritorna sempre "Contratto"

private:
    std::string tipoPolizza_;
    double premio_;
    std::string dataScadenza_;
};

#endif   // CONTRATTO_H
