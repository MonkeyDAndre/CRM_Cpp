#ifndef CLIENTE_H   // guardia di inclusione
#define CLIENTE_H

#include <string>   // tutti i campi anagrafici sono std::string

// -----------------------------------------------------------------------------
// Cliente
// -----------------------------------------------------------------------------
// Rappresenta un singolo cliente del CRM con i suoi dati anagrafici.
//
// Idea di fondo:
//  - i dati sono privati (private:) e si leggono/scrivono solo tramite metodi;
//  - ogni "setter" controlla il valore con Validatore prima di salvarlo;
//  - se il valore non e' valido il setter lancia std::invalid_argument e il
//    campo NON viene modificato (l'oggetto resta com'era).
//
// L'id NON ha un setter: viene deciso una volta sola dal costruttore (di solito
// dalla Rubrica) e non cambia piu' per tutta la vita dell'oggetto.
// -----------------------------------------------------------------------------
class Cliente {
public:
    // Costruttore: riceve tutti i campi e li imposta uno per uno riusando i
    // setter, cosi' la validazione e' identica sia alla creazione sia alla
    // modifica. Se un campo non e' valido viene lanciata un'eccezione.
    Cliente(int id, const std::string& nome, const std::string& cognome,
            const std::string& telefono, const std::string& email,
            const std::string& codiceFiscale, const std::string& dataNascita);

    // --- Getter: restituiscono una copia del campo richiesto ---
    int getId() const;                    // l'id non cambia mai, nessun rischio a restituirlo per valore
    std::string getNome() const;          // ritorno una copia della stringa, il chiamante non puo' toccare il mio membro privato
    std::string getCognome() const;
    std::string getTelefono() const;
    std::string getEmail() const;
    std::string getCodiceFiscale() const;
    std::string getDataNascita() const;

    // --- Setter: validano e poi salvano; lanciano se il valore non va bene ---
    void setNome(const std::string& nome);                      // throw std::invalid_argument se vuoto
    void setCognome(const std::string& cognome);                // throw std::invalid_argument se vuoto
    void setTelefono(const std::string& telefono);              // throw std::invalid_argument se vuoto
    void setEmail(const std::string& email);                    // throw std::invalid_argument se formato non valido
    void setCodiceFiscale(const std::string& codiceFiscale);    // throw std::invalid_argument se non 16 alfanumerici
    void setDataNascita(const std::string& dataNascita);        // throw std::invalid_argument se non gg/mm/aaaa reale

private:
    int id_;                        // identificativo univoco, assegnato dalla Rubrica
    std::string nome_;
    std::string cognome_;
    std::string telefono_;
    std::string email_;
    std::string codiceFiscale_;
    std::string dataNascita_;       // tenuta come stringa gg/mm/aaaa, non uso una classe Data apposta (fuori scope)
};

#endif   // CLIENTE_H
