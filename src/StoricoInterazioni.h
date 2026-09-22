#ifndef STORICOINTERAZIONI_H
#define STORICOINTERAZIONI_H

#include <vector>
#include <memory>    
#include <string>
#include "Interazione.h"

// -----------------------------------------------------------------------------
// StoricoInterazioni
// -----------------------------------------------------------------------------
// Contiene TUTTE le interazioni del CRM (appuntamenti e contratti insieme) in
// un'unica lista. Non ci sono liste separate per singolo cliente: le interazioni
// di un cliente si ottengono filtrando questa lista per idCliente.
//
// Perche' std::vector<std::unique_ptr<Interazione>> e non std::vector<Interazione>?
//  - Interazione e' astratta: non se ne possono creare oggetti "nudi";
//  - servono i PUNTATORI per sfruttare il polimorfismo (chiamare la stampa()
//    giusta a seconda che l'elemento sia un Appuntamento o un Contratto);
//  - unique_ptr e' un puntatore che libera da solo la memoria quando la lista
//    viene distrutta: cosi' non dobbiamo ricordarci di fare "delete".
//
// Appuntamenti e contratti condividono lo stesso contatore di id.
// -----------------------------------------------------------------------------
class StoricoInterazioni {
public:
    // Crea un Appuntamento con id automatico e lo aggiunge alla lista.
    // Ritorna l'id assegnato. Se un campo non e' valido il costruttore di
    // Appuntamento lancia std::invalid_argument e nulla viene aggiunto.
    int aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                             const std::string& data, const std::string& orario,
                             const std::string& esito, const std::string& note);

    // Come sopra ma crea un Contratto.
    int aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                          const std::string& data, const std::string& tipoPolizza,
                          double premio, const std::string& dataScadenza);

    // Ritorna i puntatori (in sola lettura) alle interazioni di un cliente.
    // Se "codiceFiscale" e' vuoto, filtra solo per idCliente; altrimenti
    // richiede che combacino sia l'id sia il codice fiscale.
    std::vector<const Interazione*> getPerCliente(int idCliente,
                                                  const std::string& codiceFiscale) const;

    // Rimuove l'interazione con quell'id. Ritorna false se non esiste.
    bool eliminaInterazione(int id);

    // Gestione del contatore di id (usati dal FileManager al caricamento).
    void impostaProssimoId(int valore);
    int getProssimoId() const;

    // Accesso in sola lettura all'intera lista (usato da FileManager e Menu).
    const std::vector<std::unique_ptr<Interazione>>& tutte() const;   // riferimento const: nessuno puo' modificare la lista da fuori

private:
    std::vector<std::unique_ptr<Interazione>> interazioni_;   // proprietaria unica di tutte le interazioni create
    int prossimoId_ = 1;   // condiviso fra appuntamenti e contratti, non due contatori separati
};

#endif   // STORICOINTERAZIONI_H
