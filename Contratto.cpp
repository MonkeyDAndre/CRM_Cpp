#include "Contratto.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>

Contratto::Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
                      const std::string& data, const std::string& tipoPolizza,
                      double premio, const std::string& dataScadenza)
    : Interazione(id, idCliente, codiceFiscaleCliente, data) {
    if (!Validatore::campoNonVuoto(tipoPolizza)) {
        throw std::invalid_argument("Tipo polizza non valido: non puo' essere vuoto");
    }
    if (!Validatore::numeroPositivo(premio)) {
        throw std::invalid_argument("Premio non valido: deve essere un numero positivo, ricevuto " +
                                     std::to_string(premio));
    }
    if (!Validatore::dataValida(dataScadenza)) {
        throw std::invalid_argument("Data di scadenza non valida: '" + dataScadenza + "'");
    }
    tipoPolizza_ = tipoPolizza;
    premio_ = premio;
    dataScadenza_ = dataScadenza;
}

std::string Contratto::getTipoPolizza() const { return tipoPolizza_; }
double Contratto::getPremio() const { return premio_; }
std::string Contratto::getDataScadenza() const { return dataScadenza_; }

std::string Contratto::toCSV() const {
    return std::to_string(getId()) + "," + std::to_string(getIdCliente()) + "," +
           getCodiceFiscaleCliente() + "," + getData() + "," + tipoPolizza_ + "," +
           std::to_string(premio_) + "," + dataScadenza_;
}

void Contratto::stampa() const {
    std::cout << "[Contratto #" << getId() << "] Cliente " << getIdCliente()
              << " - " << tipoPolizza_ << " - premio " << premio_
              << " - scadenza " << dataScadenza_ << std::endl;
}

std::string Contratto::getTipo() const {
    return "Contratto";
}