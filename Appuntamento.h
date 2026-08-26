#ifndef APPUNTAMENTO_H
#define APPUNTAMENTO_H

#include "Interazione.h"

class Appuntamento : public Interazione {
public:
    Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                 const std::string& data, const std::string& orario,
                 const std::string& esito, const std::string& note);

    std::string getOrario() const;
    std::string getEsito() const;
    std::string getNote() const;

    std::string toCSV() const override;
    void stampa() const override;
    std::string getTipo() const override;

private:
    std::string orario_;
    std::string esito_;
    std::string note_;
};

#endif