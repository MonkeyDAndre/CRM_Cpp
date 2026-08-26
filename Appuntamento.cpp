#include "Appuntamento.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>

Appuntamento::Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                            const std::string& data, const std::string& orario,
                            const std::string& esito, const std::string& note)
    : Interazione(id, idCliente, codiceFiscaleCliente, data), note_(note) {
    if (!Validatore::campoNonVuoto(orario)) {
        throw std::invalid_argument("Orario non valido: non puo' essere vuoto");
    }
    if (!Validatore::campoNonVuoto(esito)) {
        throw std::invalid_argument("Esito non valido: non puo' essere vuoto");
    }
    orario_ = orario;
    esito_ = esito;
}

std::string Appuntamento::getOrario() const { return orario_; }
std::string Appuntamento::getEsito() const { return esito_; }
std::string Appuntamento::getNote() const { return note_; }

std::string Appuntamento::toCSV() const {
    return std::to_string(getId()) + "," + std::to_string(getIdCliente()) + "," +
           getCodiceFiscaleCliente() + "," + getData() + "," + orario_ + "," + esito_ + "," +
           escapeCampoCSV(note_);
}

void Appuntamento::stampa() const {
    std::cout << "[Appuntamento #" << getId() << "] Cliente " << getIdCliente()
              << " - " << getData() << " " << orario_ << " - " << esito_;
    if (!note_.empty()) std::cout << " (" << note_ << ")";
    std::cout << std::endl;
}

std::string Appuntamento::getTipo() const {
    return "Appuntamento";
}