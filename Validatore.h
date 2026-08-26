#ifndef VALIDATORE_H
#define VALIDATORE_H

#include <string>

namespace Validatore {
    bool emailValida(const std::string& email);
    bool codiceFiscaleValido(const std::string& cf);
    bool dataValida(const std::string& data);
    bool numeroPositivo(double valore);
    bool campoNonVuoto(const std::string& valore);
    bool numeroTelefonoValido(const std::string& valore);
    bool tipoInterazioneValido(const std::string& valore);
    std::string rimuoviSpazi(std::string campo);
    std::string trim(const std::string& s);
};

#endif