#include "Interazione.h"
#include "Validatore.h"
#include <stdexcept>

Interazione::Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente,
                          const std::string& data)
    : id_(id), idCliente_(idCliente), codiceFiscaleCliente_(codiceFiscaleCliente) {
    if (!Validatore::dataValida(data)) {
        throw std::invalid_argument("Data non valida: '" + data + "' (formato atteso gg/mm/aaaa)");
    }
    data_ = data;
}

int Interazione::getId() const { return id_; }
int Interazione::getIdCliente() const { return idCliente_; }
std::string Interazione::getCodiceFiscaleCliente() const { return codiceFiscaleCliente_; }
std::string Interazione::getData() const { return data_; }

std::string Interazione::escapeCampoCSV(const std::string& campo) {
    if (campo.find(',') != std::string::npos || campo.find('"') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : campo) {
            if (c == '"') escaped += "\"\"";
            else escaped += c;
        }
        escaped += "\"";
        return escaped;
    }
    return campo;
}