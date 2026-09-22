#ifndef VALIDATORE_H   // guardia di inclusione: evita che questo header venga incollato due volte nello stesso file .cpp
#define VALIDATORE_H   // da qui in poi VALIDATORE_H e' "definito", quindi un secondo #include verra' ignorato

#include <string>   // mi serve std::string per i parametri e i valori di ritorno

// -----------------------------------------------------------------------------
// Validatore
// -----------------------------------------------------------------------------
// Raccolta di funzioni usate per controllare che i dati
// inseriti dall'utente abbiano un formato accettabile.
//
// Sono messe in un namespace (non in una classe) perche' non hanno bisogno di
// memorizzare nulla: ogni funzione riceve un valore e risponde solo true/false.
// Si richiamano scrivendo, ad esempio, Validatore::emailValida("...").
//
// Regole di validazione (volutamente semplici, come da progetto):
//  - email  : una sola '@', parte prima non vuota, dominio con un '.' interno
//  - codice fiscale : esattamente 16 caratteri, tutti lettere o cifre
//  - data   : formato gg/mm/aaaa, controllata come data reale (mesi, bisestili)
//  - numero : non negativo
//  - campo di testo : non vuoto dopo aver tolto gli spazi
// -----------------------------------------------------------------------------
namespace Validatore {  

    // Toglie gli spazi (spazi, tab, a-capo) all'inizio e alla fine della stringa.
    std::string trim(const std::string& s);  

    // true se la stringa contiene almeno un carattere "vero" (non solo spazi).
    bool campoNonVuoto(const std::string& valore); 

    // true se l'email ha una forma plausibile: "qualcosa@dominio.qualcosa".
    bool emailValida(const std::string& email); 

    // true se il codice fiscale e' lungo 16 caratteri alfanumerici.
    bool codiceFiscaleValido(const std::string& cf); 

    // true se la data e' nel formato gg/mm/aaaa ed e' un giorno realmente esistente, gestisce gli anni bisestili.
    bool dataValida(const std::string& data);

    // true se il numero non e' negativo (0 compreso).
    bool numeroPositivo(double valore);  
}

#endif  
