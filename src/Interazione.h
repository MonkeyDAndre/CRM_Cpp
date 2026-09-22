#ifndef INTERAZIONE_H
#define INTERAZIONE_H

#include <string>

// -----------------------------------------------------------------------------
// Interazione (classe base astratta)
// -----------------------------------------------------------------------------
// Rappresenta una qualsiasi interazione con un cliente. Da sola non si puo'
// creare: e' solo la "parte comune" delle due interazioni concrete del progetto
//   - Appuntamento  (una visita/telefonata della forza vendita)
//   - Contratto     (una polizza stipulata)
//
// I metodi dichiarati "= 0" sono metodi virtuali puri: ogni classe figlia DEVE
// fornirne una propria versione. E' questo che rende Interazione "astratta".
//
// Grazie al distruttore virtuale possiamo tenere Appuntamento e Contratto
// insieme in un'unica lista di puntatori a Interazione (vedi StoricoInterazioni)
// e chiamare stampa()/toCSV() senza sapere quale tipo concreto abbiamo davanti:
// e' il polimorfismo.
// -----------------------------------------------------------------------------
class Interazione {
public:
    Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente,
                const std::string& data);   // valida solo la data, gli altri campi arrivano gia' pronti dal chiamante

    // Distruttore virtuale: indispensabile quando si cancellano oggetti figli
    // tramite un puntatore a Interazione.
    virtual ~Interazione() = default;   // = default: chiedo al compilatore la versione "vuota" standard, ma resa virtuale

    // --- Dati comuni a tutte le interazioni ---
    int getId() const;
    int getIdCliente() const;
    std::string getCodiceFiscaleCliente() const; 
    std::string getData() const;

    // --- Metodi che ogni classe figlia deve implementare a modo suo ---
    virtual std::string toCSV() const = 0;   // riga da scrivere nel file CSV
    // stampa leggibile a video. Se "descrizioneCliente" e' vuota (default),
    // mostra "Cliente <id>" come sempre. Se viene passata (es. "Mario Rossi"
    // o "cliente non trovato"), la usa al posto del solo id: chi chiama con
    // il contesto di un cliente specifico puo' cosi' mostrare il nome invece
    // del solo numero, senza cambiare il comportamento di chi non lo passa.
    virtual void stampa(const std::string& descrizioneCliente = "") const = 0;
    virtual std::string getTipo() const = 0; // "Appuntamento" oppure "Contratto"

protected:
    // Utilita' condivisa dalle figlie: mette una stringa fra virgolette se
    // contiene virgole o virgolette, cosi' il CSV resta leggibile.
    static std::string escapeCampoCSV(const std::string& campo);   // protected: solo le classi figlie possono usarla, non il resto del programma

private:
    int id_;
    int idCliente_;
    std::string codiceFiscaleCliente_;   // copiato qui per ritrovare il cliente
    std::string data_;                   // formato gg/mm/aaaa
};

#endif   // INTERAZIONE_H
