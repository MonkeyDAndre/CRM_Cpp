#ifndef INTERAZIONE_H
#define INTERAZIONE_H

#include <string>

class Interazione {
public:
    Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente,
                const std::string& data);
    virtual ~Interazione() = default;

    int getId() const;
    int getIdCliente() const;
    std::string getCodiceFiscaleCliente() const;
    std::string getData() const;

    virtual std::string toCSV() const = 0;
    virtual void stampa() const = 0;
    virtual std::string getTipo() const = 0;

protected:
    static std::string escapeCampoCSV(const std::string& campo);

private:
    int id_;
    int idCliente_;
    std::string codiceFiscaleCliente_;
    std::string data_;
};

#endif