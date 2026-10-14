#ifndef MENU_H
#define MENU_H

#include <string>
#include "Rubrica.h"
#include "StoricoInterazioni.h"

// -----------------------------------------------------------------------------
// Menu
// -----------------------------------------------------------------------------
// Si occupa SOLO del dialogo con l'utente: stampa i menu, legge cio' che viene
// digitato e chiama i metodi giusti su Rubrica e StoricoInterazioni.
// Non contiene logica di business e non tocca i file direttamente: delega a
// FileManager, richiamato dopo ogni modifica andata a buon fine e all'uscita.
//
// Rubrica e StoricoInterazioni sono tenuti per RIFERIMENTO (&): il Menu non ne
// possiede una copia, lavora sugli stessi oggetti creati nel main.
// -----------------------------------------------------------------------------
class Menu {
public:
    Menu(Rubrica& rubrica, StoricoInterazioni& storico);   // il Menu non crea i dati, li riceve gia' pronti dal main

    // Avvia il ciclo del menu principale; ritorna quando l'utente sceglie "Esci"
    // oppure quando lo standard input termina (in entrambi i casi salva prima).
    void eseguiMenuPrincipale();

private:
    Rubrica& rubrica_;              // riferimento, non copia: stesso oggetto per tutta la vita del programma
    StoricoInterazioni& storico_;   // idem

    // Il ciclo vero e proprio del menu principale (eseguiMenuPrincipale lo
    // avvolge per gestire la fine dell'input).
    void cicloMenuPrincipale();

    // --- Sottomenu "Gestione Clienti" e relative operazioni ---
    void eseguiSottomenuClienti();
    void aggiungiClienteInterattivo();
    void visualizzaTuttiIClienti();
    void modificaClienteInterattivo();
    void eliminaClienteInterattivo();
    void cercaClienteInterattivo();
    // Stampa l'elenco clienti nel formato usato da "visualizza/modifica/elimina"
    // (condiviso per non ripetere lo stesso ciclo in tre punti diversi).
    static void stampaListaClienti(const std::vector<Cliente>& clienti);

    // --- Sottomenu "Gestione Interazioni" e relative operazioni ---
    void eseguiSottomenuInterazioni();
    void aggiungiAppuntamentoInterattivo();
    void aggiungiContrattoInterattivo();
    void visualizzaInterazioniClienteInterattivo();
    void cercaInterazioniInterattivo();

    // Riscrive tutti i CSV con lo stato attuale (chiamata dopo ogni modifica).
    void salvaDati();
    // Salva tutto su file e stampa il messaggio di uscita.
    void salvaESci();

    // --- Piccoli aiutanti per leggere l'input ---
    // Legge un numero di scelta dal menu; ritorna -1 se l'utente non ha
    // digitato un numero valido.
    static int leggiSceltaMenu();
    // Stampa "prompt" e restituisce l'intera riga digitata dall'utente.
    static std::string leggiRiga(const std::string& prompt);   // static: non ha bisogno di accedere a rubrica_/storico_
    // Chiede un ID e lo converte: ritorna false (e avvisa) se non e' un intero valido.
    static bool leggiId(const std::string& prompt, int& id);
    // Ripete "domanda (s/n)" finche' la risposta non e' s oppure n.
    static bool chiediConferma(const std::string& domanda);

    // --- Aiutanti per la presentazione delle schermate ---
    // Pulisce il terminale (comando diverso a seconda del sistema operativo).
    static void pulisciSchermo();
    // Pulisce il terminale e stampa il titolo della schermata corrente.
    // "permetteAnnulla": se true, stampa anche il suggerimento su come annullare
    // l'inserimento (usato dalle schermate che chiedono almeno un valore).
    static void mostraSchermata(const std::string& titolo, bool permetteAnnulla = false);
    // Stampa un messaggio e aspetta che l'utente prema Invio, cosi' il risultato
    // di un'azione resta visibile finche' non decide lui di andare avanti.
    static void attendiInvio();
};

#endif   // MENU_H
