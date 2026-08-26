#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
public:
    Cliente(int id, const std::string& nome, const std::string& cognome,
            const std::string& telefono, const std::string& email,
            const std::string& codiceFiscale, const std::string& dataNascita);

    int getId();
    std::string getNome();
    std::string getCognome();
    std::string getTelefono();
    std::string getEmail();
    std::string getCodiceFiscale();
    std::string getDataNascita();

    void setNome(const std::string& nome);
    void setCognome(const std::string& cognome);
    void setTelefono(const std::string& telefono);
    void setEmail(const std::string& email);
    void setCodiceFiscale(const std::string& codiceFiscale);
    void setDataNascita(const std::string& dataNascita);

private:
    int id_;
    std::string nome_;
    std::string cognome_;
    std::string telefono_;
    std::string email_;
    std::string codiceFiscale_;
    std::string dataNascita_;
};

#endif