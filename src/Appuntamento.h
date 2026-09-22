#ifndef APPUNTAMENTO_H
#define APPUNTAMENTO_H

#include "Interazione.h"

// -----------------------------------------------------------------------------
// Appuntamento : public Interazione
// -----------------------------------------------------------------------------
// Interazione concreta: un incontro/contatto della forza vendita con il cliente.
// Oltre ai dati comuni (id, idCliente, codice fiscale, data) aggiunge:
//   - orario  (es. "10:30")
//   - esito   (es. "concluso", "da ricontattare", "annullato")
//   - note    (testo libero, puo' essere vuoto)
//
// "public Interazione" significa: un Appuntamento E' una Interazione, quindi puo'
// essere usato ovunque sia richiesta una Interazione.
// -----------------------------------------------------------------------------
class Appuntamento : public Interazione {
public:
    Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                 const std::string& data, const std::string& orario,
                 const std::string& esito, const std::string& note);

    std::string getOrario() const;   // getter dei soli campi aggiunti da questa classe
    std::string getEsito() const;
    std::string getNote() const;

    // Versioni specifiche per l'Appuntamento dei metodi della classe base.
    std::string toCSV() const override;   // override: implemento davvero il virtuale puro della classe madre
    void stampa(const std::string& descrizioneCliente = "") const override;
    std::string getTipo() const override;   // ritorna sempre "Appuntamento"

private:
    std::string orario_;
    std::string esito_;
    std::string note_;
};

#endif   // APPUNTAMENTO_H
