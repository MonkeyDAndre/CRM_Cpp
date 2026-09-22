#include "Validatore.h"   // dichiarazioni delle funzioni che implemento qui sotto
#include <cctype>       

namespace Validatore {  
    // -------------------------------------------------------------------------
    // trim: rimuove gli spazi iniziali e finali
    // -------------------------------------------------------------------------
    std::string trim(const std::string& s) {
        const std::string spazi = " \t\n\r";   // elenco dei caratteri che considero "spazio bianco"

        // Posizione del primo carattere che NON e' uno spazio.
        size_t inizio = s.find_first_not_of(spazi);   // find_first_not_of scorre da sinistra finche' non trova un carattere fuori dall'elenco

        // Se non ne trova nessuno, la stringa e' fatta solo di spazi: torna "".
        if (inizio == std::string::npos) {   // npos e' il valore speciale che find_* restituisce quando non trova nulla
            return "";                       // stringa vuota, coerente con "non c'e' contenuto vero"
        }

        // Posizione dell'ultimo carattere che NON e' uno spazio.
        size_t fine = s.find_last_not_of(spazi);   // stessa idea ma partendo da destra

        // Estrae la porzione compresa fra "inizio" e "fine" (inclusi).
        return s.substr(inizio, fine - inizio + 1);   // substr() prende come parametri l'indice del primo carattere e la lunghezza della sottostringa da estrarre
    }

    // -------------------------------------------------------------------------
    // campoNonVuoto: true se resta qualcosa dopo aver tolto gli spazi
    // -------------------------------------------------------------------------
    bool campoNonVuoto(const std::string& valore) {
        return !trim(valore).empty();   // faccio il trim e controllo se e' rimasto vuoto: se e' vuoto il campo NON va bene
    }

    // -------------------------------------------------------------------------
    // emailValida: controllo semplificato
    // -------------------------------------------------------------------------
    // Passi:
    //  1. togliere gli spazi ai bordi
    //  2. deve esserci esattamente una '@'
    //  3. la parte prima della '@' non puo' essere vuota
    //  4. nella parte dopo la '@' deve esserci un '.' che non sia il primo
    //     ne' l'ultimo carattere (cosi' "dominio" ed "estensione" esistono)
    bool emailValida(const std::string& email) {
        std::string e = trim(email);   // passo 1: niente spazi accidentali copiati/incollati dall'utente

        // Passo 2: conta le '@' e memorizza la posizione dell'ultima trovata.
        int numeroChiocciole = 0;      // contatore delle '@' incontrate
        size_t posChiocciola = 0;      // ricordo dove si trova (mi servira' per tagliare la stringa)
        for (size_t i = 0; i < e.size(); ++i) {   // scorro carattere per carattere
            if (e[i] == '@') {                    // trovata una chiocciola
                numeroChiocciole++;               // la conto
                posChiocciola = i;                // e salvo la sua posizione (se ce ne fosse piu' di una, resta l'ultima)
            }
        }
        if (numeroChiocciole != 1) {   // voglio ESATTAMENTE una '@'
            return false;
        }

        // Passo 3: parte locale = tutto cio' che sta prima della '@'.
        std::string parteLocale = e.substr(0, posChiocciola);   // dall'inizio fino alla posizione della '@' (esclusa)
        if (parteLocale.empty()) {   // "@dominio.com" non e' un indirizzo valido
            return false;
        }

        // Passo 4: dominio = tutto cio' che sta dopo la '@'
        std::string dominio = e.substr(posChiocciola + 1);   // dalla '@' in poi, saltando la '@' stessa
        size_t posPunto = dominio.find('.');   // cerco il primo punto nel dominio
        if (posPunto == std::string::npos) {
            return false;                       // nessun punto nel dominio -> niente estensione, rifiuto
        }
        if (posPunto == 0 || posPunto == dominio.size() - 1) {
            return false;                       // punto all'inizio ("@ .com") o alla fine ("@dominio.") -> rifiuto
        }

        return true;   // passati tutti i controlli, la considero plausibile (non garantisco che esista davvero)
    }

    // -------------------------------------------------------------------------
    // codiceFiscaleValido: 16 caratteri, tutti lettere o cifre
    // -------------------------------------------------------------------------

    bool codiceFiscaleValido(const std::string& cf) {
        std::string c = trim(cf);   // tolgo eventuali spazi ai bordi prima di contare i caratteri

        if (c.size() != 16) {   // il codice fiscale italiano e' sempre lungo 16 caratteri
            return false;
        }
        for (char carattere : c) {   // range-based for: scorro ogni carattere della stringa uno a uno
            // isalnum vuole un unsigned char per evitare comportamenti strani
            // con caratteri accentati: lo convertiamo prima di passarlo.
            if (!std::isalnum(static_cast<unsigned char>(carattere))) {   // se anche un solo carattere non e' lettera/cifra...
                return false;                                            // ...tutto il codice fiscale e' da rifiutare
            }
        }
        return true;   // lunghezza giusta e tutti caratteri alfanumerici: lo accetto
    }

    // -------------------------------------------------------------------------
    // dataValida: formato gg/mm/aaaa e giorno realmente esistente
    // -------------------------------------------------------------------------
    bool dataValida(const std::string& data) {
        std::string d = trim(data);   // niente spazi accidentali prima/dopo

        // Passo 1: la lunghezza deve essere esattamente 10 (gg/mm/aaaa).
        if (d.size() != 10) {   // "01/01/2000" ha esattamente 10 caratteri, qualunque altra lunghezza e' gia' sbagliata
            return false;
        }

        // Passo 2: nelle posizioni 2 e 5 ci devono essere le barre.
        if (d[2] != '/' || d[5] != '/') {   // indice 2 e indice 5 sono dove mi aspetto i due separatori "/"
            return false;
        }

        // Passo 3: tutte le altre 8 posizioni devono essere cifre.
        //          (indici 0,1  giorno) (3,4 mese) (6,7,8,9 anno)
        int posizioniCifra[8] = {0, 1, 3, 4, 6, 7, 8, 9};   // elenco esplicito degli indici che devono contenere numeri
        for (int pos : posizioniCifra) {                    // controllo ciascuna di quelle posizioni
            if (!std::isdigit(static_cast<unsigned char>(d[pos]))) {   // se anche una sola non e' una cifra...
                return false;                                          // ...la data e' malformata
            }
        }

        // Passo 4: convertiamo le sottostringhe in numeri interi.
        int giorno = std::stoi(d.substr(0, 2));   // caratteri 0-1 -> giorno
        int mese   = std::stoi(d.substr(3, 2));   // caratteri 3-4 -> mese
        int anno   = std::stoi(d.substr(6, 4));   // caratteri 6-9 -> anno (4 cifre)

        // Passo 5: controlli di intervallo su mese e anno.
        if (mese < 1 || mese > 12) {   // il mese deve stare fra gennaio (1) e dicembre (12)
            return false;
        }
        if (anno < 1900 || anno > 2999) {   // range "ragionevole", scelto per evitare anni assurdi (0000, 9999...)
            return false;
        }

        // Passo 6: quanti giorni ha questo mese?
        int giorniDelMese;   // qui calcolo il massimo giorno ammesso per il mese/anno indicati
        if (mese == 2) {
            // Febbraio: 29 giorni negli anni bisestili, 28 altrimenti.
            bool bisestile = (anno % 4 == 0 && anno % 100 != 0) || (anno % 400 == 0);   // regola classica dei bisestili
            giorniDelMese = bisestile ? 29 : 28;   // operatore ternario: se bisestile 29, altrimenti 28
        } else if (mese == 4 || mese == 6 || mese == 9 || mese == 11) {
            giorniDelMese = 30;                 // aprile, giugno, settembre, novembre hanno 30 giorni
        } else {
            giorniDelMese = 31;                 // tutti gli altri mesi (gennaio, marzo, maggio, ...) hanno 31 giorni
        }

        // Passo 7: il giorno deve stare fra 1 e i giorni del mese.
        if (giorno < 1 || giorno > giorniDelMese) {   // es. 31/04 non esiste perche' aprile arriva solo a 30
            return false;
        }

        return true;   // passati tutti i controlli: e' una data reale nel formato giusto
    }

    // -------------------------------------------------------------------------
    // numeroPositivo: true se il valore non e' negativo
    // -------------------------------------------------------------------------
    bool numeroPositivo(double valore) {
        return valore >= 0;   // nota per me: il nome dice "positivo" ma qui accetto anche lo 0 (>= e non >)
    }
}
