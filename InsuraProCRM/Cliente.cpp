#include "Cliente.h"
#include "Validatore.h"
#include <stdexcept>

Cliente::Cliente(int id, const std::string& nome, const std::string& cognome,
                  const std::string& telefono, const std::string& email,
                  const std::string& codiceFiscale, const std::string& dataNascita)
    : id_(id) {
    setNome(nome);
    setCognome(cognome);
    setTelefono(telefono);
    setEmail(email);
    setCodiceFiscale(codiceFiscale);
    setDataNascita(dataNascita);
}

int Cliente::getId() { return id_; }
std::string Cliente::getNome() { return nome_; }
std::string Cliente::getCognome()  { return cognome_; }
std::string Cliente::getTelefono() { return telefono_; }
std::string Cliente::getEmail() { return email_; }
std::string Cliente::getCodiceFiscale() { return codiceFiscale_; }
std::string Cliente::getDataNascita() { return dataNascita_; }

void Cliente::setNome(const std::string& nome) {
    if (!Validatore::campoNonVuoto(nome)) {
        throw std::invalid_argument("Nome non valido: non puo' essere vuoto");
    }
    nome_ = nome;
}

void Cliente::setCognome(const std::string& cognome) {
    if (!Validatore::campoNonVuoto(cognome)) {
        throw std::invalid_argument("Cognome non valido: non puo' essere vuoto");
    }
    cognome_ = cognome;
}

void Cliente::setTelefono(const std::string& telefono) {
    if (!Validatore::campoNonVuoto(telefono)) {
        throw std::invalid_argument("Telefono non valido: non puo' essere vuoto");
    }
    telefono_ = telefono;
}

void Cliente::setEmail(const std::string& email) {
    if (!Validatore::emailValida(email)) {
        throw std::invalid_argument("Email non valida: '" + email + "'");
    }
    email_ = email;
}

void Cliente::setCodiceFiscale(const std::string& codiceFiscale) {
    if (!Validatore::codiceFiscaleValido(codiceFiscale)) {
        throw std::invalid_argument(
            "Codice fiscale non valido: deve essere alfanumerico di 16 caratteri, ricevuto '" +
            codiceFiscale + "'");
    }
    codiceFiscale_ = codiceFiscale;
}

void Cliente::setDataNascita(const std::string& dataNascita) {
    if (!Validatore::dataValida(dataNascita)) {
        throw std::invalid_argument(
            "Data di nascita non valida: '" + dataNascita + "' (formato atteso gg/mm/aaaa)");
    }
    dataNascita_ = dataNascita;
}