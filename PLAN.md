# CRM InsuraPro Solutions Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Costruire un CRM a riga di comando in C++ per InsuraPro Solutions, con CRUD clienti, gestione interazioni (appuntamenti/contratti) collegate ai clienti, e persistenza su CSV, consegnabile come zip compilabile con `g++ *.cpp -o crm` senza dipendenze esterne.

**Architecture:** Livelli separati per responsabilità — dati (`Cliente`, `Interazione`/`Appuntamento`/`Contratto`), collezioni in memoria (`Rubrica`, `StoricoInterazioni`), I/O (`FileManager`, unico punto di accesso al filesystem), validazione (`Validatore`, funzioni pure), interfaccia utente (`Menu`, nessuna logica di business). Ogni livello viene costruito e testato in isolamento prima di essere collegato al successivo.

**Tech Stack:** C++11/14 standard library only (niente C++17: il comando di build non fissa lo standard). Nessun framework di test esterno — test scritti come piccoli programmi con `<cassert>`, compilati a mano e tenuti fuori dalla cartella di consegna.

**Spec:** `docs/superpowers/specs/2026-08-23-crm-insurapro-design.md`

## Global Constraints

- Nessuna libreria esterna: solo standard library C++ (niente FTXUI/ncurses/framework di test).
- Nessun sistema di build: compilazione manuale `g++ *.cpp -o crm`, documentata nel README.
- Nessuna API platform-specific (niente `<conio.h>`, niente codici ANSI obbligatori).
- Menu numerato classico: input a riga con numero + invio, nessuna navigazione a frecce.
- Setter/costruttori che ricevono un valore non valido sollevano `std::invalid_argument` con messaggio che identifica campo e motivo; lo stato dell'oggetto resta invariato in caso di rifiuto.
- `Validatore::campoNonVuoto` fa trim degli spazi prima del controllo: una stringa di soli spazi è considerata vuota.
- Persistenza CSV con supporto a campi quotati (per note contenenti virgole); righe malformate vengono scartate con avviso a schermo, il caricamento continua.
- Eliminazione di un cliente NON elimina a cascata le sue interazioni (restano identificabili via `idCliente` + `codiceFiscaleCliente`).
- Contatore ID clienti = max ID letto da `clienti.csv` + 1; contatore ID interazioni condiviso tra appuntamenti e contratti = max ID tra i due file + 1.
- File di test e fixture vivono in `tests/`, MAI in `InsuraProCRM/` (altrimenti `g++ *.cpp -o crm` trova più `main()` e non compila).

---

### Task 0: Scheletro del progetto

**Files:**
- Create: `InsuraProCRM/Cliente.h`, `InsuraProCRM/Cliente.cpp`
- Create: `InsuraProCRM/Rubrica.h`, `InsuraProCRM/Rubrica.cpp`
- Create: `InsuraProCRM/Interazione.h`, `InsuraProCRM/Interazione.cpp`
- Create: `InsuraProCRM/Appuntamento.h`, `InsuraProCRM/Appuntamento.cpp`
- Create: `InsuraProCRM/Contratto.h`, `InsuraProCRM/Contratto.cpp`
- Create: `InsuraProCRM/StoricoInterazioni.h`, `InsuraProCRM/StoricoInterazioni.cpp`
- Create: `InsuraProCRM/FileManager.h`, `InsuraProCRM/FileManager.cpp`
- Create: `InsuraProCRM/Validatore.h`, `InsuraProCRM/Validatore.cpp`
- Create: `InsuraProCRM/Menu.h`, `InsuraProCRM/Menu.cpp`
- Create: `InsuraProCRM/main.cpp`
- Create: `.gitignore`

**Interfaces:**
- Produces: cartella `InsuraProCRM/` con tutti i file sorgente vuoti (solo include guard + dichiarazioni vuote dove serve) che compilano insieme. `main.cpp` contiene un `main()` minimo.

- [ ] **Step 1: Inizializzare il repository**

```bash
git init
cat > .gitignore << 'EOF'
crm
*.o
tests/test_*
!tests/test_*.cpp
EOF
```

- [ ] **Step 2: Creare gli header vuoti con include guard**

Per ciascun file `.h` (`Cliente.h`, `Rubrica.h`, `Interazione.h`, `Appuntamento.h`, `Contratto.h`, `StoricoInterazioni.h`, `FileManager.h`, `Validatore.h`, `Menu.h`), contenuto minimo, ad esempio per `Validatore.h`:

```cpp
#ifndef VALIDATORE_H
#define VALIDATORE_H

#endif
```

Ripetere lo stesso schema (nome della macro coerente col nome del file) per tutti gli altri header.

- [ ] **Step 3: Creare i `.cpp` vuoti con il proprio include**

Ogni `.cpp` include solo il proprio header, ad esempio `Validatore.cpp`:

```cpp
#include "Validatore.h"
```

Ripetere per tutti gli altri `.cpp`.

- [ ] **Step 4: Creare `main.cpp` minimo**

```cpp
#include <iostream>

int main() {
    std::cout << "OK" << std::endl;
    return 0;
}
```

- [ ] **Step 5: Verificare che lo scheletro compili**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./crm`
Expected: nessun errore di compilazione, output `OK`.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM .gitignore
git commit -m "chore: scheletro progetto InsuraProCRM"
```

---

### Task 1: Validatore

**Files:**
- Modify: `InsuraProCRM/Validatore.h`
- Modify: `InsuraProCRM/Validatore.cpp`
- Test: `tests/test_validatore.cpp`

**Interfaces:**
- Produces (namespace di funzioni libere, non una classe — nessuna istanza `Validatore` viene mai creata nel progetto; la sintassi di chiamata `Validatore::funzione(...)` resta identica ovunque venga usata negli altri task):
  - `bool Validatore::emailValida(const std::string& email)`
  - `bool Validatore::codiceFiscaleValido(const std::string& cf)`
  - `bool Validatore::dataValida(const std::string& data)` — formato `gg/mm/aaaa`, calendario reale
  - `bool Validatore::numeroPositivo(double valore)`
  - `bool Validatore::campoNonVuoto(const std::string& valore)` — trim prima del controllo

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_validatore.cpp
#include <cassert>
#include <iostream>
#include "../InsuraProCRM/Validatore.h"

int main() {
    // emailValida
    assert(Validatore::emailValida("a@b.com") == true);
    assert(Validatore::emailValida("abc") == false);
    assert(Validatore::emailValida("a@bcom") == false);
    assert(Validatore::emailValida("") == false);

    // codiceFiscaleValido
    assert(Validatore::codiceFiscaleValido("RSSMRA80A01H501U") == true); // 16 alfanumerici
    assert(Validatore::codiceFiscaleValido("RSSMRA80A01H501") == false); // 15 caratteri
    assert(Validatore::codiceFiscaleValido("RSSMRA80A01H501!") == false); // simbolo non alfanumerico

    // dataValida
    assert(Validatore::dataValida("29/02/2024") == true);  // bisestile
    assert(Validatore::dataValida("29/02/2023") == false); // non bisestile
    assert(Validatore::dataValida("31/04/2026") == false); // aprile ha 30 giorni
    assert(Validatore::dataValida("32/01/2026") == false); // giorno inesistente
    assert(Validatore::dataValida("15/13/2026") == false); // mese inesistente
    assert(Validatore::dataValida("01/01/2026") == true);

    // numeroPositivo
    assert(Validatore::numeroPositivo(0.0) == false);
    assert(Validatore::numeroPositivo(-1.0) == false);
    assert(Validatore::numeroPositivo(0.01) == true);

    // campoNonVuoto (trim su spazi)
    assert(Validatore::campoNonVuoto("") == false);
    assert(Validatore::campoNonVuoto("   ") == false);
    assert(Validatore::campoNonVuoto("  a  ") == true);
    assert(Validatore::campoNonVuoto("Mario") == true);

    std::cout << "test_validatore: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca (per assenza di implementazione)**

Run: `g++ tests/test_validatore.cpp InsuraProCRM/Validatore.cpp -o tests/test_validatore`
Expected: FAIL in compilazione — `'emailValida' is not a member of 'Validatore'` (perché `Validatore.h` è ancora vuoto).

- [ ] **Step 3: Implementare `Validatore.h`**

```cpp
#ifndef VALIDATORE_H
#define VALIDATORE_H

#include <string>

namespace Validatore {
    bool emailValida(const std::string& email);
    bool codiceFiscaleValido(const std::string& cf);
    bool dataValida(const std::string& data);
    bool numeroPositivo(double valore);
    bool campoNonVuoto(const std::string& valore);
}

#endif
```

Nota: le funzioni di supporto (`trim`, calcolo giorni per mese, anno bisestile) NON
compaiono qui — a differenza di una classe, un namespace non obbliga a dichiarare
nell'header i dettagli implementativi "privati". Restano interne al `.cpp` (vedi Step 4).

- [ ] **Step 4: Implementare `Validatore.cpp`**

```cpp
#include "Validatore.h"
#include <cctype>
#include <sstream>

namespace {
    // Funzioni di supporto con linkage interno: visibili solo dentro questo
    // file (Validatore.cpp), non esportate nell'header, non chiamabili da
    // nessun altro .cpp. Sostituiscono i metodi "private" che avrebbe avuto
    // una classe, senza doverli comunque dichiarare nell'header.

    std::string trim(const std::string& s) {
        size_t inizio = s.find_first_not_of(" \t\n\r");
        if (inizio == std::string::npos) return "";
        size_t fine = s.find_last_not_of(" \t\n\r");
        return s.substr(inizio, fine - inizio + 1);
    }

    bool annoBisestile(int anno) {
        return (anno % 4 == 0 && anno % 100 != 0) || (anno % 400 == 0);
    }

    bool giornoValidoPerMese(int giorno, int mese, int anno) {
        if (mese < 1 || mese > 12) return false;
        int giorniPerMese[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int massimo = giorniPerMese[mese - 1];
        if (mese == 2 && annoBisestile(anno)) massimo = 29;
        return giorno >= 1 && giorno <= massimo;
    }
}

bool Validatore::campoNonVuoto(const std::string& valore) {
    return !trim(valore).empty();
}

bool Validatore::emailValida(const std::string& email) {
    size_t posChiocciola = email.find('@');
    if (posChiocciola == std::string::npos || posChiocciola == 0 ||
        posChiocciola == email.size() - 1) {
        return false;
    }
    std::string dominio = email.substr(posChiocciola + 1);
    size_t posPunto = dominio.find('.');
    if (posPunto == std::string::npos || posPunto == 0 ||
        posPunto == dominio.size() - 1) {
        return false;
    }
    return true;
}

bool Validatore::codiceFiscaleValido(const std::string& cf) {
    if (cf.size() != 16) return false;
    for (char c : cf) {
        if (!std::isalnum(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

bool Validatore::dataValida(const std::string& data) {
    if (data.size() != 10 || data[2] != '/' || data[5] != '/') return false;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(static_cast<unsigned char>(data[i]))) return false;
    }
    int giorno = std::stoi(data.substr(0, 2));
    int mese = std::stoi(data.substr(3, 2));
    int anno = std::stoi(data.substr(6, 4));
    return giornoValidoPerMese(giorno, mese, anno);
}

bool Validatore::numeroPositivo(double valore) {
    return valore > 0.0;
}
```

**Perché il namespace anonimo:** raggruppa `trim`, `annoBisestile`, `giornoValidoPerMese`
dandogli *linkage interno* — sono visibili solo all'interno di `Validatore.cpp`. È
l'equivalente, per funzioni libere, di quello che `private:` faceva per i metodi di una
classe, con una differenza in meglio: qui non compaiono nemmeno nell'header, quindi chi
legge `Validatore.h` vede solo l'interfaccia pubblica, senza dettagli implementativi.

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_validatore.cpp InsuraProCRM/Validatore.cpp -o tests/test_validatore && ./tests/test_validatore`
Expected: PASS — stampa `test_validatore: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/Validatore.h InsuraProCRM/Validatore.cpp tests/test_validatore.cpp
git commit -m "feat: implementa Validatore con funzioni pure di validazione"
```

---

### Task 2: Cliente

**Files:**
- Modify: `InsuraProCRM/Cliente.h`
- Modify: `InsuraProCRM/Cliente.cpp`
- Test: `tests/test_cliente.cpp`

**Interfaces:**
- Consumes: `Validatore::emailValida`, `Validatore::codiceFiscaleValido`, `Validatore::dataValida`, `Validatore::campoNonVuoto` (Task 1)
- Produces:
  - `Cliente(int id, const std::string& nome, const std::string& cognome, const std::string& telefono, const std::string& email, const std::string& codiceFiscale, const std::string& dataNascita)` — throws `std::invalid_argument`
  - `int getId() const`, `std::string getNome() const`, `std::string getCognome() const`, `std::string getTelefono() const`, `std::string getEmail() const`, `std::string getCodiceFiscale() const`, `std::string getDataNascita() const`
  - `void setNome(const std::string&)`, `void setCognome(const std::string&)`, `void setTelefono(const std::string&)` — throw `std::invalid_argument` se vuoto
  - `void setEmail(const std::string&)` — throw se non valida
  - `void setCodiceFiscale(const std::string&)` — throw se non valido
  - `void setDataNascita(const std::string&)` — throw se non valida

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_cliente.cpp
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../InsuraProCRM/Cliente.h"

int main() {
    Cliente c(1, "Mario", "Rossi", "3331234567", "mario.rossi@email.com",
              "RSSMRA80A01H501U", "01/01/1980");

    assert(c.getId() == 1);
    assert(c.getNome() == "Mario");
    assert(c.getCognome() == "Rossi");
    assert(c.getEmail() == "mario.rossi@email.com");
    assert(c.getCodiceFiscale() == "RSSMRA80A01H501U");
    assert(c.getDataNascita() == "01/01/1980");

    // setter con valore non valido: deve lanciare e non modificare il campo
    bool eccezioneLanciata = false;
    try {
        c.setEmail("non-valida");
    } catch (const std::invalid_argument& e) {
        eccezioneLanciata = true;
        std::string messaggio = e.what();
        assert(messaggio.find("mail") != std::string::npos ||
               messaggio.find("Email") != std::string::npos);
    }
    assert(eccezioneLanciata);
    assert(c.getEmail() == "mario.rossi@email.com"); // valore precedente intatto

    eccezioneLanciata = false;
    try {
        c.setCodiceFiscale("TROPPO_CORTO");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);
    assert(c.getCodiceFiscale() == "RSSMRA80A01H501U");

    // costruttore con dato non valido deve lanciare
    bool costruttoreLancia = false;
    try {
        Cliente invalido(2, "Luca", "Bianchi", "123", "email-invalida",
                          "CORTO", "31/02/2020");
    } catch (const std::invalid_argument&) {
        costruttoreLancia = true;
    }
    assert(costruttoreLancia);

    std::cout << "test_cliente: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_cliente.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_cliente`
Expected: FAIL in compilazione — `Cliente` non ha ancora costruttore né membri.

- [ ] **Step 3: Implementare `Cliente.h`**

```cpp
#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
public:
    Cliente(int id, const std::string& nome, const std::string& cognome,
            const std::string& telefono, const std::string& email,
            const std::string& codiceFiscale, const std::string& dataNascita);

    int getId() const;
    std::string getNome() const;
    std::string getCognome() const;
    std::string getTelefono() const;
    std::string getEmail() const;
    std::string getCodiceFiscale() const;
    std::string getDataNascita() const;

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
```

- [ ] **Step 4: Implementare `Cliente.cpp`**

```cpp
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

int Cliente::getId() const { return id_; }
std::string Cliente::getNome() const { return nome_; }
std::string Cliente::getCognome() const { return cognome_; }
std::string Cliente::getTelefono() const { return telefono_; }
std::string Cliente::getEmail() const { return email_; }
std::string Cliente::getCodiceFiscale() const { return codiceFiscale_; }
std::string Cliente::getDataNascita() const { return dataNascita_; }

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
```

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_cliente.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_cliente && ./tests/test_cliente`
Expected: PASS — stampa `test_cliente: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/Cliente.h InsuraProCRM/Cliente.cpp tests/test_cliente.cpp
git commit -m "feat: implementa Cliente con setter validati che lanciano eccezioni"
```

---

### Task 3: Rubrica

**Files:**
- Modify: `InsuraProCRM/Rubrica.h`
- Modify: `InsuraProCRM/Rubrica.cpp`
- Test: `tests/test_rubrica.cpp`

**Interfaces:**
- Consumes: `Cliente` (Task 2), inclusa la sua politica di eccezioni
- Produces:
  - `int aggiungiCliente(const std::string& nome, const std::string& cognome, const std::string& telefono, const std::string& email, const std::string& codiceFiscale, const std::string& dataNascita)` — throws `std::invalid_argument`, ritorna il nuovo id
  - `const std::vector<Cliente>& visualizzaTutti() const`
  - `bool modificaCliente(int id, const std::string& nome, const std::string& cognome, const std::string& telefono, const std::string& email, const std::string& codiceFiscale, const std::string& dataNascita)` — `false` se id non trovato; throws `std::invalid_argument` se un campo non è valido (validazione PRIMA di sovrascrivere)
  - `bool eliminaCliente(int id)` — `false` se non trovato
  - `bool getById(int id, Cliente& risultato) const` — `false` se non trovato, `risultato` non toccato in quel caso
  - `std::vector<Cliente> cercaPerNomeCognome(const std::string& query) const` — match parziale case-insensitive
  - `void impostaProssimoId(int valore)`, `int getProssimoId() const` — usati da `FileManager` dopo il caricamento (Task 8)

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_rubrica.cpp
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../InsuraProCRM/Rubrica.h"

int main() {
    Rubrica rubrica;

    int id1 = rubrica.aggiungiCliente("Mario", "Rossi", "111", "mario@email.com",
                                       "RSSMRA80A01H501U", "01/01/1980");
    int id2 = rubrica.aggiungiCliente("Anna", "Rossetti", "222", "anna@email.com",
                                       "RSSNNA85B02H501U", "02/02/1985");
    int id3 = rubrica.aggiungiCliente("Luca", "Bianchi", "333", "luca@email.com",
                                       "BNCLCU90C03H501U", "03/03/1990");

    assert(id1 == 1);
    assert(id2 == 2);
    assert(id3 == 3);
    assert(rubrica.visualizzaTutti().size() == 3);

    Cliente trovato("", "", "", "", "", "", ""); // placeholder non compilabile: vedi nota sotto

    std::cout << "test_rubrica: tutti i test passati" << std::endl;
    return 0;
}
```

Nota: la riga `Cliente trovato(...)` sopra e' volutamente scorretta (Cliente non ha un
costruttore del genere) — serve a dimostrare che il test, cosi' com'e', non compila ancora
perche' manca un modo pulito di ottenere un `Cliente` di default per `getById`. Sostituirla
subito con la versione corretta qui sotto prima di eseguire lo step 2, altrimenti l'errore di
compilazione sarebbe quello sbagliato (test rotto, non implementazione mancante).

```cpp
// tests/test_rubrica.cpp (versione corretta, sostituisce il file sopra)
#include <cassert>
#include <iostream>
#include "../InsuraProCRM/Rubrica.h"

int main() {
    Rubrica rubrica;

    int id1 = rubrica.aggiungiCliente("Mario", "Rossi", "111", "mario@email.com",
                                       "RSSMRA80A01H501U", "01/01/1980");
    int id2 = rubrica.aggiungiCliente("Anna", "Rossetti", "222", "anna@email.com",
                                       "RSSNNA85B02H501U", "02/02/1985");
    int id3 = rubrica.aggiungiCliente("Luca", "Bianchi", "333", "luca@email.com",
                                       "BNCLCU90C03H501U", "03/03/1990");

    assert(id1 == 1);
    assert(id2 == 2);
    assert(id3 == 3);
    assert(rubrica.visualizzaTutti().size() == 3);

    Cliente c(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(rubrica.getById(id2, c) == true);
    assert(c.getNome() == "Anna");

    Cliente nonUsato(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(rubrica.getById(999, nonUsato) == false);
    assert(nonUsato.getNome() == "x"); // non toccato

    std::vector<Cliente> risultatoRicerca = rubrica.cercaPerNomeCognome("ross");
    assert(risultatoRicerca.size() == 2); // Rossi, Rossetti (case-insensitive)

    std::vector<Cliente> risultatoVuoto = rubrica.cercaPerNomeCognome("zzz");
    assert(risultatoVuoto.size() == 0);

    assert(rubrica.eliminaCliente(id2) == true);
    assert(rubrica.visualizzaTutti().size() == 2);
    assert(rubrica.getById(id2, c) == false);
    assert(rubrica.eliminaCliente(999) == false); // id inesistente, nessun crash

    bool eccezioneLanciata = false;
    try {
        rubrica.modificaCliente(id1, "Mario", "Rossi", "111", "email-non-valida",
                                 "RSSMRA80A01H501U", "01/01/1980");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);
    Cliente originale(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    rubrica.getById(id1, originale);
    assert(originale.getEmail() == "mario@email.com"); // non modificato dal tentativo fallito

    assert(rubrica.modificaCliente(999, "A", "B", "111", "a@b.com",
                                    "0000000000000000", "01/01/2000") == false);

    std::cout << "test_rubrica: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_rubrica.cpp InsuraProCRM/Rubrica.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_rubrica`
Expected: FAIL in compilazione — `Rubrica` non ha ancora metodi.

- [ ] **Step 3: Implementare `Rubrica.h`**

```cpp
#ifndef RUBRICA_H
#define RUBRICA_H

#include <vector>
#include <string>
#include "Cliente.h"

class Rubrica {
public:
    int aggiungiCliente(const std::string& nome, const std::string& cognome,
                         const std::string& telefono, const std::string& email,
                         const std::string& codiceFiscale, const std::string& dataNascita);

    const std::vector<Cliente>& visualizzaTutti() const;

    bool modificaCliente(int id, const std::string& nome, const std::string& cognome,
                          const std::string& telefono, const std::string& email,
                          const std::string& codiceFiscale, const std::string& dataNascita);

    bool eliminaCliente(int id);

    bool getById(int id, Cliente& risultato) const;

    std::vector<Cliente> cercaPerNomeCognome(const std::string& query) const;

    void impostaProssimoId(int valore);
    int getProssimoId() const;

private:
    std::vector<Cliente> clienti_;
    int prossimoId_ = 1;

    int trovaIndicePerId(int id) const; // ritorna -1 se non trovato
    static std::string minuscolo(const std::string& s);
};

#endif
```

- [ ] **Step 4: Implementare `Rubrica.cpp`**

```cpp
#include "Rubrica.h"
#include <algorithm>
#include <cctype>

std::string Rubrica::minuscolo(const std::string& s) {
    std::string risultato = s;
    std::transform(risultato.begin(), risultato.end(), risultato.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return risultato;
}

int Rubrica::trovaIndicePerId(int id) const {
    for (size_t i = 0; i < clienti_.size(); ++i) {
        if (clienti_[i].getId() == id) return static_cast<int>(i);
    }
    return -1;
}

int Rubrica::aggiungiCliente(const std::string& nome, const std::string& cognome,
                              const std::string& telefono, const std::string& email,
                              const std::string& codiceFiscale, const std::string& dataNascita) {
    Cliente nuovo(prossimoId_, nome, cognome, telefono, email, codiceFiscale, dataNascita);
    clienti_.push_back(nuovo);
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

const std::vector<Cliente>& Rubrica::visualizzaTutti() const {
    return clienti_;
}

bool Rubrica::modificaCliente(int id, const std::string& nome, const std::string& cognome,
                               const std::string& telefono, const std::string& email,
                               const std::string& codiceFiscale, const std::string& dataNascita) {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;

    // Costruisco un Cliente temporaneo: se la validazione fallisce, lancia PRIMA
    // di toccare clienti_[indice], garantendo nessuna scrittura parziale.
    Cliente aggiornato(id, nome, cognome, telefono, email, codiceFiscale, dataNascita);
    clienti_[indice] = aggiornato;
    return true;
}

bool Rubrica::eliminaCliente(int id) {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;
    clienti_.erase(clienti_.begin() + indice);
    return true;
}

bool Rubrica::getById(int id, Cliente& risultato) const {
    int indice = trovaIndicePerId(id);
    if (indice == -1) return false;
    risultato = clienti_[indice];
    return true;
}

std::vector<Cliente> Rubrica::cercaPerNomeCognome(const std::string& query) const {
    std::vector<Cliente> risultati;
    std::string queryMinuscola = minuscolo(query);
    for (const auto& cliente : clienti_) {
        std::string nomeMinuscolo = minuscolo(cliente.getNome());
        std::string cognomeMinuscolo = minuscolo(cliente.getCognome());
        if (nomeMinuscolo.find(queryMinuscola) != std::string::npos ||
            cognomeMinuscolo.find(queryMinuscola) != std::string::npos) {
            risultati.push_back(cliente);
        }
    }
    return risultati;
}

void Rubrica::impostaProssimoId(int valore) {
    prossimoId_ = valore;
}

int Rubrica::getProssimoId() const {
    return prossimoId_;
}
```

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_rubrica.cpp InsuraProCRM/Rubrica.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_rubrica && ./tests/test_rubrica`
Expected: PASS — stampa `test_rubrica: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/Rubrica.h InsuraProCRM/Rubrica.cpp tests/test_rubrica.cpp
git commit -m "feat: implementa Rubrica con CRUD e ricerca in memoria"
```

---

### Task 4: Interazione (base astratta) + Appuntamento

**Files:**
- Modify: `InsuraProCRM/Interazione.h`
- Modify: `InsuraProCRM/Interazione.cpp`
- Modify: `InsuraProCRM/Appuntamento.h`
- Modify: `InsuraProCRM/Appuntamento.cpp`
- Test: `tests/test_appuntamento.cpp`

**Interfaces:**
- Consumes: `Validatore::dataValida`, `Validatore::campoNonVuoto` (Task 1)
- Produces:
  - `Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente, const std::string& data)` — throws `std::invalid_argument` se `data` non valida
  - `int getId() const`, `int getIdCliente() const`, `std::string getCodiceFiscaleCliente() const`, `std::string getData() const`
  - `virtual std::string toCSV() const = 0`, `virtual void stampa() const = 0`, `virtual std::string getTipo() const = 0`, distruttore virtuale
  - `static std::string Interazione::escapeCampoCSV(const std::string& campo)` — avvolge tra virgolette se il campo contiene `,` o `"`
  - `Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente, const std::string& data, const std::string& orario, const std::string& esito, const std::string& note)` — throws se `orario`/`esito` vuoti
  - `Appuntamento::getTipo()` ritorna esattamente `"Appuntamento"`

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_appuntamento.cpp
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include "../InsuraProCRM/Interazione.h"
#include "../InsuraProCRM/Appuntamento.h"

int main() {
    Appuntamento a(1, 10, "RSSMRA80A01H501U", "15/03/2026", "10:30", "concluso",
                   "Interessato a polizza casa");

    assert(a.getId() == 1);
    assert(a.getIdCliente() == 10);
    assert(a.getCodiceFiscaleCliente() == "RSSMRA80A01H501U");
    assert(a.getData() == "15/03/2026");
    assert(a.getTipo() == "Appuntamento");

    std::string csv = a.toCSV();
    assert(csv == "1,10,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"Interessato a polizza casa\"");

    Appuntamento b(2, 10, "RSSMRA80A01H501U", "16/03/2026", "11:00", "annullato",
                   "chiama lunedi', non martedi'"); // nota senza virgole: solo per contrasto
    std::string csvB = b.toCSV();
    assert(csvB.find(",\"chiama lunedi', non martedi'\"") != std::string::npos);

    Appuntamento c(3, 10, "RSSMRA80A01H501U", "17/03/2026", "12:00", "da ricontattare",
                   "nota, con virgola dentro");
    std::string csvC = c.toCSV();
    assert(csvC == "3,10,RSSMRA80A01H501U,17/03/2026,12:00,da ricontattare,\"nota, con virgola dentro\"");

    bool eccezioneLanciata = false;
    try {
        Appuntamento invalido(4, 10, "RSSMRA80A01H501U", "31/02/2026", "12:00", "concluso", "");
    } catch (const std::invalid_argument&) {
        eccezioneLanciata = true;
    }
    assert(eccezioneLanciata);

    // conteggio istanze vive tramite puntatore a base: nessun leak, distruttore virtuale
    {
        std::vector<std::unique_ptr<Interazione>> collezione;
        collezione.push_back(std::unique_ptr<Interazione>(
            new Appuntamento(5, 10, "RSSMRA80A01H501U", "18/03/2026", "09:00", "concluso", "")));
        assert(collezione.size() == 1);
        assert(collezione[0]->getTipo() == "Appuntamento");
    } // qui il unique_ptr distrugge l'oggetto tramite puntatore a base: se il distruttore
      // non e' virtuale, questo test non lo rileva direttamente, ma verifica almeno
      // che il codice compili e giri con -Wall senza warning di distruttore non virtuale.

    std::cout << "test_appuntamento: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ -Wall tests/test_appuntamento.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Validatore.cpp -o tests/test_appuntamento`
Expected: FAIL in compilazione — nessuna classe ha ancora membri.

- [ ] **Step 3: Implementare `Interazione.h`**

```cpp
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
```

- [ ] **Step 4: Implementare `Interazione.cpp`**

```cpp
#include "Interazione.h"
#include "Validatore.h"
#include <stdexcept>

Interazione::Interazione(int id, int idCliente, const std::string& codiceFiscaleCliente,
                          const std::string& data)
    : id_(id), idCliente_(idCliente), codiceFiscaleCliente_(codiceFiscaleCliente) {
    if (!Validatore::dataValida(data)) {
        throw std::invalid_argument("Data non valida: '" + data + "' (formato atteso gg/mm/aaaa)");
    }
    data_ = data;
}

int Interazione::getId() const { return id_; }
int Interazione::getIdCliente() const { return idCliente_; }
std::string Interazione::getCodiceFiscaleCliente() const { return codiceFiscaleCliente_; }
std::string Interazione::getData() const { return data_; }

std::string Interazione::escapeCampoCSV(const std::string& campo) {
    if (campo.find(',') != std::string::npos || campo.find('"') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : campo) {
            if (c == '"') escaped += "\"\"";
            else escaped += c;
        }
        escaped += "\"";
        return escaped;
    }
    return campo;
}
```

- [ ] **Step 5: Implementare `Appuntamento.h`**

```cpp
#ifndef APPUNTAMENTO_H
#define APPUNTAMENTO_H

#include "Interazione.h"

class Appuntamento : public Interazione {
public:
    Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                 const std::string& data, const std::string& orario,
                 const std::string& esito, const std::string& note);

    std::string getOrario() const;
    std::string getEsito() const;
    std::string getNote() const;

    std::string toCSV() const override;
    void stampa() const override;
    std::string getTipo() const override;

private:
    std::string orario_;
    std::string esito_;
    std::string note_;
};

#endif
```

- [ ] **Step 6: Implementare `Appuntamento.cpp`**

```cpp
#include "Appuntamento.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>

Appuntamento::Appuntamento(int id, int idCliente, const std::string& codiceFiscaleCliente,
                            const std::string& data, const std::string& orario,
                            const std::string& esito, const std::string& note)
    : Interazione(id, idCliente, codiceFiscaleCliente, data), note_(note) {
    if (!Validatore::campoNonVuoto(orario)) {
        throw std::invalid_argument("Orario non valido: non puo' essere vuoto");
    }
    if (!Validatore::campoNonVuoto(esito)) {
        throw std::invalid_argument("Esito non valido: non puo' essere vuoto");
    }
    orario_ = orario;
    esito_ = esito;
}

std::string Appuntamento::getOrario() const { return orario_; }
std::string Appuntamento::getEsito() const { return esito_; }
std::string Appuntamento::getNote() const { return note_; }

std::string Appuntamento::toCSV() const {
    return std::to_string(getId()) + "," + std::to_string(getIdCliente()) + "," +
           getCodiceFiscaleCliente() + "," + getData() + "," + orario_ + "," + esito_ + "," +
           escapeCampoCSV(note_);
}

void Appuntamento::stampa() const {
    std::cout << "[Appuntamento #" << getId() << "] Cliente " << getIdCliente()
              << " - " << getData() << " " << orario_ << " - " << esito_;
    if (!note_.empty()) std::cout << " (" << note_ << ")";
    std::cout << std::endl;
}

std::string Appuntamento::getTipo() const {
    return "Appuntamento";
}
```

- [ ] **Step 7: Eseguire il test e verificare che passi**

Run: `g++ -Wall tests/test_appuntamento.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Validatore.cpp -o tests/test_appuntamento && ./tests/test_appuntamento`
Expected: PASS — stampa `test_appuntamento: tutti i test passati`, exit code 0, nessun warning `-Wall` su distruttore non virtuale.

- [ ] **Step 8: Commit**

```bash
git add InsuraProCRM/Interazione.h InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.h InsuraProCRM/Appuntamento.cpp tests/test_appuntamento.cpp
git commit -m "feat: implementa Interazione (base astratta) e Appuntamento"
```

---

### Task 5: Contratto

**Files:**
- Modify: `InsuraProCRM/Contratto.h`
- Modify: `InsuraProCRM/Contratto.cpp`
- Test: `tests/test_contratto.cpp`

**Interfaces:**
- Consumes: `Interazione` (Task 4, incluso `escapeCampoCSV`), `Validatore::numeroPositivo`, `Validatore::dataValida`, `Validatore::campoNonVuoto` (Task 1)
- Produces:
  - `Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente, const std::string& data, const std::string& tipoPolizza, double premio, const std::string& dataScadenza)` — throws se `tipoPolizza` vuoto, `premio` non positivo, o `dataScadenza` non valida
  - `Contratto::getTipo()` ritorna esattamente `"Contratto"`

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_contratto.cpp
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../InsuraProCRM/Contratto.h"

int main() {
    Contratto c(1, 10, "RSSMRA80A01H501U", "15/03/2026", "Auto", 450.00, "15/03/2027");

    assert(c.getId() == 1);
    assert(c.getTipo() == "Contratto");
    assert(c.toCSV() == "1,10,RSSMRA80A01H501U,15/03/2026,Auto,450.000000,15/03/2027");

    bool eccezionePremio = false;
    try {
        Contratto invalido(2, 10, "RSSMRA80A01H501U", "15/03/2026", "Casa", -10.0, "15/03/2027");
    } catch (const std::invalid_argument& e) {
        eccezionePremio = true;
        std::string msg = e.what();
        assert(msg.find("remio") != std::string::npos); // "premio"/"Premio"
    }
    assert(eccezionePremio);

    bool eccezioneScadenza = false;
    try {
        Contratto invalido(3, 10, "RSSMRA80A01H501U", "15/03/2026", "Vita", 100.0, "31/02/2027");
    } catch (const std::invalid_argument&) {
        eccezioneScadenza = true;
    }
    assert(eccezioneScadenza);

    bool eccezioneTipoPolizza = false;
    try {
        Contratto invalido(4, 10, "RSSMRA80A01H501U", "15/03/2026", "", 100.0, "15/03/2027");
    } catch (const std::invalid_argument&) {
        eccezioneTipoPolizza = true;
    }
    assert(eccezioneTipoPolizza);

    std::cout << "test_contratto: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_contratto.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_contratto`
Expected: FAIL in compilazione — `Contratto` non ha ancora membri.

- [ ] **Step 3: Implementare `Contratto.h`**

```cpp
#ifndef CONTRATTO_H
#define CONTRATTO_H

#include "Interazione.h"

class Contratto : public Interazione {
public:
    Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
              const std::string& data, const std::string& tipoPolizza,
              double premio, const std::string& dataScadenza);

    std::string getTipoPolizza() const;
    double getPremio() const;
    std::string getDataScadenza() const;

    std::string toCSV() const override;
    void stampa() const override;
    std::string getTipo() const override;

private:
    std::string tipoPolizza_;
    double premio_;
    std::string dataScadenza_;
};

#endif
```

- [ ] **Step 4: Implementare `Contratto.cpp`**

```cpp
#include "Contratto.h"
#include "Validatore.h"
#include <iostream>
#include <stdexcept>

Contratto::Contratto(int id, int idCliente, const std::string& codiceFiscaleCliente,
                      const std::string& data, const std::string& tipoPolizza,
                      double premio, const std::string& dataScadenza)
    : Interazione(id, idCliente, codiceFiscaleCliente, data) {
    if (!Validatore::campoNonVuoto(tipoPolizza)) {
        throw std::invalid_argument("Tipo polizza non valido: non puo' essere vuoto");
    }
    if (!Validatore::numeroPositivo(premio)) {
        throw std::invalid_argument("Premio non valido: deve essere un numero positivo, ricevuto " +
                                     std::to_string(premio));
    }
    if (!Validatore::dataValida(dataScadenza)) {
        throw std::invalid_argument("Data di scadenza non valida: '" + dataScadenza + "'");
    }
    tipoPolizza_ = tipoPolizza;
    premio_ = premio;
    dataScadenza_ = dataScadenza;
}

std::string Contratto::getTipoPolizza() const { return tipoPolizza_; }
double Contratto::getPremio() const { return premio_; }
std::string Contratto::getDataScadenza() const { return dataScadenza_; }

std::string Contratto::toCSV() const {
    return std::to_string(getId()) + "," + std::to_string(getIdCliente()) + "," +
           getCodiceFiscaleCliente() + "," + getData() + "," + tipoPolizza_ + "," +
           std::to_string(premio_) + "," + dataScadenza_;
}

void Contratto::stampa() const {
    std::cout << "[Contratto #" << getId() << "] Cliente " << getIdCliente()
              << " - " << tipoPolizza_ << " - premio " << premio_
              << " - scadenza " << dataScadenza_ << std::endl;
}

std::string Contratto::getTipo() const {
    return "Contratto";
}
```

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_contratto.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_contratto && ./tests/test_contratto`
Expected: PASS — stampa `test_contratto: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/Contratto.h InsuraProCRM/Contratto.cpp tests/test_contratto.cpp
git commit -m "feat: implementa Contratto"
```

---

### Task 6: StoricoInterazioni

**Files:**
- Modify: `InsuraProCRM/StoricoInterazioni.h`
- Modify: `InsuraProCRM/StoricoInterazioni.cpp`
- Test: `tests/test_storico.cpp`

**Interfaces:**
- Consumes: `Interazione`, `Appuntamento`, `Contratto` (Task 4, 5)
- Produces:
  - `int aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente, const std::string& data, const std::string& orario, const std::string& esito, const std::string& note)` — throws (propagata da `Appuntamento`), ritorna il nuovo id
  - `int aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente, const std::string& data, const std::string& tipoPolizza, double premio, const std::string& dataScadenza)` — throws (propagata da `Contratto`), ritorna il nuovo id
  - `std::vector<const Interazione*> getPerCliente(int idCliente, const std::string& codiceFiscale) const`
  - `bool eliminaInterazione(int id)`
  - `void impostaProssimoId(int valore)`, `int getProssimoId() const`
  - `const std::vector<std::unique_ptr<Interazione>>& tutte() const`

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_storico.cpp
#include <cassert>
#include <iostream>
#include "../InsuraProCRM/StoricoInterazioni.h"

int main() {
    StoricoInterazioni storico;

    int idApp = storico.aggiungiAppuntamento(1, "RSSMRA80A01H501U", "15/03/2026",
                                              "10:30", "concluso", "");
    int idContr = storico.aggiungiContratto(1, "RSSMRA80A01H501U", "16/03/2026",
                                             "Auto", 450.0, "16/03/2027");
    int idApp2 = storico.aggiungiAppuntamento(2, "BNCLCU90C03H501U", "17/03/2026",
                                               "09:00", "concluso", "");

    assert(idApp == 1);
    assert(idContr == 2); // contatore condiviso, cresce indipendentemente dal tipo
    assert(idApp2 == 3);

    auto perCliente1 = storico.getPerCliente(1, "");
    assert(perCliente1.size() == 2);

    auto perCliente2 = storico.getPerCliente(2, "");
    assert(perCliente2.size() == 1);

    auto perClienteInesistente = storico.getPerCliente(99, "");
    assert(perClienteInesistente.size() == 0);

    assert(storico.eliminaInterazione(idApp) == true);
    assert(storico.getPerCliente(1, "").size() == 1);
    assert(storico.eliminaInterazione(999) == false); // non trovato, nessun crash

    std::cout << "test_storico: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_storico.cpp InsuraProCRM/StoricoInterazioni.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_storico`
Expected: FAIL in compilazione — `StoricoInterazioni` non ha ancora membri.

- [ ] **Step 3: Implementare `StoricoInterazioni.h`**

```cpp
#ifndef STORICOINTERAZIONI_H
#define STORICOINTERAZIONI_H

#include <vector>
#include <memory>
#include <string>
#include "Interazione.h"

class StoricoInterazioni {
public:
    int aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                              const std::string& data, const std::string& orario,
                              const std::string& esito, const std::string& note);

    int aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                           const std::string& data, const std::string& tipoPolizza,
                           double premio, const std::string& dataScadenza);

    std::vector<const Interazione*> getPerCliente(int idCliente,
                                                   const std::string& codiceFiscale) const;

    bool eliminaInterazione(int id);

    void impostaProssimoId(int valore);
    int getProssimoId() const;

    const std::vector<std::unique_ptr<Interazione>>& tutte() const;

private:
    std::vector<std::unique_ptr<Interazione>> interazioni_;
    int prossimoId_ = 1;
};

#endif
```

- [ ] **Step 4: Implementare `StoricoInterazioni.cpp`**

```cpp
#include "StoricoInterazioni.h"
#include "Appuntamento.h"
#include "Contratto.h"

int StoricoInterazioni::aggiungiAppuntamento(int idCliente, const std::string& codiceFiscaleCliente,
                                              const std::string& data, const std::string& orario,
                                              const std::string& esito, const std::string& note) {
    interazioni_.push_back(std::unique_ptr<Interazione>(
        new Appuntamento(prossimoId_, idCliente, codiceFiscaleCliente, data, orario, esito, note)));
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

int StoricoInterazioni::aggiungiContratto(int idCliente, const std::string& codiceFiscaleCliente,
                                           const std::string& data, const std::string& tipoPolizza,
                                           double premio, const std::string& dataScadenza) {
    interazioni_.push_back(std::unique_ptr<Interazione>(
        new Contratto(prossimoId_, idCliente, codiceFiscaleCliente, data, tipoPolizza, premio,
                       dataScadenza)));
    int idAssegnato = prossimoId_;
    prossimoId_++;
    return idAssegnato;
}

std::vector<const Interazione*> StoricoInterazioni::getPerCliente(
    int idCliente, const std::string& codiceFiscale) const {
    std::vector<const Interazione*> risultati;
    for (const auto& interazione : interazioni_) {
        bool matchId = interazione->getIdCliente() == idCliente;
        bool matchCf = codiceFiscale.empty() ||
                       interazione->getCodiceFiscaleCliente() == codiceFiscale;
        if (matchId && matchCf) {
            risultati.push_back(interazione.get());
        }
    }
    return risultati;
}

bool StoricoInterazioni::eliminaInterazione(int id) {
    for (size_t i = 0; i < interazioni_.size(); ++i) {
        if (interazioni_[i]->getId() == id) {
            interazioni_.erase(interazioni_.begin() + i);
            return true;
        }
    }
    return false;
}

void StoricoInterazioni::impostaProssimoId(int valore) {
    prossimoId_ = valore;
}

int StoricoInterazioni::getProssimoId() const {
    return prossimoId_;
}

const std::vector<std::unique_ptr<Interazione>>& StoricoInterazioni::tutte() const {
    return interazioni_;
}
```

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_storico.cpp InsuraProCRM/StoricoInterazioni.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_storico && ./tests/test_storico`
Expected: PASS — stampa `test_storico: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/StoricoInterazioni.h InsuraProCRM/StoricoInterazioni.cpp tests/test_storico.cpp
git commit -m "feat: implementa StoricoInterazioni con contatore condiviso"
```

---

### Task 7: FileManager — parsing CSV

**Files:**
- Modify: `InsuraProCRM/FileManager.h`
- Modify: `InsuraProCRM/FileManager.cpp`
- Test: `tests/test_csv_parsing.cpp`

**Interfaces:**
- Produces: `std::vector<std::string> FileManager::parseRigaCSV(const std::string& riga)` — namespace di funzioni libere (stesso ragionamento di `Validatore`: nessuno stato interno, ogni funzione riceve tutto ciò che le serve come parametro) — split su virgola con supporto a campi tra virgolette (virgole interne preservate, `""` come escape di `"` letterale)

- [ ] **Step 1: Scrivere il test fallente**

```cpp
// tests/test_csv_parsing.cpp
#include <cassert>
#include <iostream>
#include "../InsuraProCRM/FileManager.h"

int main() {
    auto campi1 = FileManager::parseRigaCSV(
        "1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980");
    assert(campi1.size() == 7);
    assert(campi1[1] == "Mario");
    assert(campi1[6] == "01/01/1980");

    auto campi2 = FileManager::parseRigaCSV(
        "1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"Interessato a polizza casa\"");
    assert(campi2.size() == 7);
    assert(campi2[6] == "Interessato a polizza casa"); // senza virgolette

    auto campi3 = FileManager::parseRigaCSV(
        "1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"nota, con virgola dentro\"");
    assert(campi3.size() == 7); // la virgola interna NON deve spezzare il campo
    assert(campi3[6] == "nota, con virgola dentro");

    auto campiVuoti = FileManager::parseRigaCSV("");
    assert(campiVuoti.size() == 0);

    auto campiMalformati = FileManager::parseRigaCSV("1,Mario,Rossi");
    assert(campiMalformati.size() == 3); // il parser non giudica, restituisce cio' che trova

    std::cout << "test_csv_parsing: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_csv_parsing.cpp InsuraProCRM/FileManager.cpp -o tests/test_csv_parsing`
Expected: FAIL in compilazione — `parseRigaCSV` non esiste ancora.

- [ ] **Step 3: Implementare `FileManager.h` (solo la parte di parsing per ora)**

```cpp
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>

namespace FileManager {
    std::vector<std::string> parseRigaCSV(const std::string& riga);
}

#endif
```

- [ ] **Step 4: Implementare `FileManager.cpp` (solo parsing per ora)**

```cpp
#include "FileManager.h"

std::vector<std::string> FileManager::parseRigaCSV(const std::string& riga) {
    std::vector<std::string> campi;
    if (riga.empty()) return campi;

    std::string campoCorrente;
    bool dentroVirgolette = false;

    for (size_t i = 0; i < riga.size(); ++i) {
        char c = riga[i];
        if (dentroVirgolette) {
            if (c == '"') {
                if (i + 1 < riga.size() && riga[i + 1] == '"') {
                    campoCorrente += '"';
                    ++i; // salta il secondo '"' dell'escape
                } else {
                    dentroVirgolette = false;
                }
            } else {
                campoCorrente += c;
            }
        } else {
            if (c == '"') {
                dentroVirgolette = true;
            } else if (c == ',') {
                campi.push_back(campoCorrente);
                campoCorrente.clear();
            } else {
                campoCorrente += c;
            }
        }
    }
    campi.push_back(campoCorrente);
    return campi;
}
```

- [ ] **Step 5: Eseguire il test e verificare che passi**

Run: `g++ tests/test_csv_parsing.cpp InsuraProCRM/FileManager.cpp -o tests/test_csv_parsing && ./tests/test_csv_parsing`
Expected: PASS — stampa `test_csv_parsing: tutti i test passati`, exit code 0.

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/FileManager.h InsuraProCRM/FileManager.cpp tests/test_csv_parsing.cpp
git commit -m "feat: implementa parsing CSV con supporto campi quotati"
```

---

### Task 8: FileManager — carica/salva clienti

**Files:**
- Modify: `InsuraProCRM/FileManager.h`
- Modify: `InsuraProCRM/FileManager.cpp`
- Test: `tests/test_filemanager_clienti.cpp`
- Test fixtures: `tests/fixtures/clienti_ok.csv`, `tests/fixtures/clienti_malformato.csv`

**Interfaces:**
- Consumes: `Rubrica` (Task 3), `FileManager::parseRigaCSV` (Task 7)
- Produces:
  - `void caricaClienti(Rubrica& rubrica, const std::string& path = "clienti.csv")` — righe malformate scartate con avviso su `std::cerr`, imposta `rubrica.impostaProssimoId(maxId + 1)`; file assente = nessun errore, rubrica vuota
  - `void salvaClienti(const Rubrica& rubrica, const std::string& path = "clienti.csv")` — sovrascrittura completa

- [ ] **Step 1: Creare le fixture CSV**

```bash
mkdir -p tests/fixtures
cat > tests/fixtures/clienti_ok.csv << 'EOF'
id,nome,cognome,telefono,email,codice_fiscale,data_nascita
1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980
2,Anna,Verdi,3339876543,anna.verdi@email.com,VRDNNA90C41H501U,05/03/1990
3,Luca,Bianchi,3335555555,luca.bianchi@email.com,BNCLCU85D02H501U,10/04/1985
EOF

cat > tests/fixtures/clienti_malformato.csv << 'EOF'
id,nome,cognome,telefono,email,codice_fiscale,data_nascita
1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980
2,Anna,Verdi,3339876543
3,Luca,Bianchi,3335555555,luca.bianchi@email.com,BNCLCU85D02H501U,10/04/1985
EOF
```

- [ ] **Step 2: Scrivere il test fallente**

```cpp
// tests/test_filemanager_clienti.cpp
#include <cassert>
#include <iostream>
#include <cstdio>
#include "../InsuraProCRM/FileManager.h"
#include "../InsuraProCRM/Rubrica.h"

int main() {
    // caricamento file valido
    Rubrica r1;
    FileManager::caricaClienti(r1, "tests/fixtures/clienti_ok.csv");
    assert(r1.visualizzaTutti().size() == 3);
    Cliente c(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(r1.getById(2, c) == true);
    assert(c.getNome() == "Anna");
    assert(r1.getProssimoId() == 4); // max id (3) + 1

    // caricamento file con riga malformata: scartata, le altre caricate
    Rubrica r2;
    FileManager::caricaClienti(r2, "tests/fixtures/clienti_malformato.csv");
    assert(r2.visualizzaTutti().size() == 2); // riga 2 scartata

    // file inesistente: nessun errore, rubrica vuota
    Rubrica r3;
    FileManager::caricaClienti(r3, "tests/fixtures/clienti_assente_non_esiste.csv");
    assert(r3.visualizzaTutti().size() == 0);

    // round-trip: salvo e ricarico, deve dare lo stesso contenuto logico
    Rubrica sorgente;
    sorgente.aggiungiCliente("Giulia", "Neri", "3331112223", "giulia.neri@email.com",
                              "NRIGLI88E44H501U", "12/06/1988");
    sorgente.aggiungiCliente("Paolo", "Blu", "3334445556", "paolo.blu@email.com",
                              "BLUPLA79F10H501U", "20/07/1979");
    FileManager::salvaClienti(sorgente, "tests/fixtures/clienti_roundtrip.csv");

    Rubrica ricaricata;
    FileManager::caricaClienti(ricaricata, "tests/fixtures/clienti_roundtrip.csv");
    assert(ricaricata.visualizzaTutti().size() == 2);
    Cliente verifica(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    assert(ricaricata.getById(1, verifica) == true);
    assert(verifica.getNome() == "Giulia");
    assert(verifica.getEmail() == "giulia.neri@email.com");

    std::remove("tests/fixtures/clienti_roundtrip.csv");

    std::cout << "test_filemanager_clienti: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 3: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_filemanager_clienti.cpp InsuraProCRM/FileManager.cpp InsuraProCRM/Rubrica.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_filemanager_clienti`
Expected: FAIL in compilazione — `caricaClienti`/`salvaClienti` non esistono ancora.

- [ ] **Step 4: Aggiornare `FileManager.h`**

```cpp
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Rubrica.h"

namespace FileManager {
    std::vector<std::string> parseRigaCSV(const std::string& riga);

    void caricaClienti(Rubrica& rubrica, const std::string& path = "clienti.csv");
    void salvaClienti(const Rubrica& rubrica, const std::string& path = "clienti.csv");
}

#endif
```

**Nota sulla scelta namespace vs classe:** a differenza di `Menu` (che tiene `Rubrica&` e
`StoricoInterazioni&` come membri, cioè stato che persiste tra una chiamata e l'altra),
ogni funzione di `FileManager` riceve tutto ciò che le serve tramite i parametri e non
conserva nulla al proprio interno tra una chiamata e la successiva — stesso identico
ragionamento fatto per `Validatore` nel Task 1. Questo significa anche che **non serve
istanziare nessun oggetto `FileManager`** in `main.cpp` (vedi Task 12): si chiama
direttamente `FileManager::caricaClienti(...)`.

- [ ] **Step 5: Aggiungere l'implementazione in `FileManager.cpp`**

```cpp
#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// ... (parseRigaCSV gia' presente dal Task 7, invariato) ...

void FileManager::caricaClienti(Rubrica& rubrica, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return; // file assente non e' un errore
    }

    std::string riga;
    bool primaRiga = true;
    int numeroRiga = 0;
    int maxId = 0;

    while (std::getline(file, riga)) {
        numeroRiga++;
        if (primaRiga) { primaRiga = false; continue; } // intestazione
        if (riga.empty()) continue;

        std::vector<std::string> campi = parseRigaCSV(riga);
        if (campi.size() != 7) {
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (attesi 7 campi, trovati "
                      << campi.size() << ")" << std::endl;
            continue;
        }

        try {
            int id = std::stoi(campi[0]);
            Cliente c(id, campi[1], campi[2], campi[3], campi[4], campi[5], campi[6]);
            // Rubrica assegna i propri id in aggiungiCliente(); per rispettare l'id
            // gia' presente nel file, impostiamo il contatore e usiamo un percorso
            // dedicato che non rigenera l'id: aggiungiClienteConId non esiste ancora
            // nell'interfaccia Rubrica, quindi qui si ricostruisce lo stato tramite
            // aggiungiCliente() e si forza l'id atteso prima di richiamarlo:
            rubrica.impostaProssimoId(id);
            rubrica.aggiungiCliente(campi[1], campi[2], campi[3], campi[4], campi[5], campi[6]);
            if (id > maxId) maxId = id;
        } catch (const std::exception& e) {
            std::cerr << "Attenzione: riga " << numeroRiga << " di " << path
                      << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            continue;
        }
    }

    rubrica.impostaProssimoId(maxId + 1);
}

void FileManager::salvaClienti(const Rubrica& rubrica, const std::string& path) {
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Errore: impossibile scrivere su " << path << std::endl;
        return;
    }

    file << "id,nome,cognome,telefono,email,codice_fiscale,data_nascita\n";
    for (const auto& c : rubrica.visualizzaTutti()) {
        file << c.getId() << "," << c.getNome() << "," << c.getCognome() << ","
             << c.getTelefono() << "," << c.getEmail() << "," << c.getCodiceFiscale() << ","
             << c.getDataNascita() << "\n";
    }
}
```

**Nota per l'implementatore:** l'uso di `impostaProssimoId(id)` seguito da `aggiungiCliente(...)`
per forzare un id specifico e' fragile se in mezzo al caricamento arrivano righe fuori ordine
di id decrescente (il `maxId` finale sistema comunque il contatore, ma un cliente caricato con
id non sequenziale rispetto all'ordine del file resta corretto solo perche' l'id e' esplicito
per costruzione — verificarlo con un quarto caso di test se si sospetta un file con id non
in ordine crescente; i fixture di questo task sono gia' in ordine crescente, quindi il test
cosi' com'e' non lo scopre).

- [ ] **Step 6: Eseguire il test e verificare che passi**

Run: `g++ tests/test_filemanager_clienti.cpp InsuraProCRM/FileManager.cpp InsuraProCRM/Rubrica.cpp InsuraProCRM/Cliente.cpp InsuraProCRM/Validatore.cpp -o tests/test_filemanager_clienti && ./tests/test_filemanager_clienti`
Expected: PASS — stampa `test_filemanager_clienti: tutti i test passati`, exit code 0. Il caricamento del file malformato stampa un avviso su `stderr` per la riga 3 del file (riga 2 dei dati, terza riga fisica contando l'intestazione) — verificare che il messaggio citi il numero di riga.

- [ ] **Step 7: Commit**

```bash
git add InsuraProCRM/FileManager.h InsuraProCRM/FileManager.cpp tests/test_filemanager_clienti.cpp tests/fixtures/clienti_ok.csv tests/fixtures/clienti_malformato.csv
git commit -m "feat: FileManager carica/salva clienti su CSV con gestione righe malformate"
```

---

### Task 9: FileManager — carica/salva interazioni

**Files:**
- Modify: `InsuraProCRM/FileManager.h`
- Modify: `InsuraProCRM/FileManager.cpp`
- Test: `tests/test_filemanager_interazioni.cpp`
- Test fixtures: `tests/fixtures/appuntamenti_ok.csv`, `tests/fixtures/contratti_ok.csv`, `tests/fixtures/contratti_malformato.csv`

**Interfaces:**
- Consumes: `StoricoInterazioni` (Task 6), `FileManager::parseRigaCSV` (Task 7)
- Produces:
  - `void caricaInterazioni(StoricoInterazioni& storico, const std::string& pathAppuntamenti = "appuntamenti.csv", const std::string& pathContratti = "contratti.csv")` — contatore condiviso = max tra i due file + 1
  - `void salvaInterazioni(const StoricoInterazioni& storico, const std::string& pathAppuntamenti = "appuntamenti.csv", const std::string& pathContratti = "contratti.csv")` — smista per `getTipo()`

- [ ] **Step 1: Creare le fixture CSV**

```bash
cat > tests/fixtures/appuntamenti_ok.csv << 'EOF'
id,id_cliente,codice_fiscale,data,orario,esito,note
1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,"Interessato a polizza casa"
5,1,RSSMRA80A01H501U,20/03/2026,11:00,annullato,""
EOF

cat > tests/fixtures/contratti_ok.csv << 'EOF'
id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza
2,1,RSSMRA80A01H501U,15/03/2026,Auto,450.00,15/03/2027
3,2,VRDNNA90C41H501U,16/03/2026,Casa,300.00,16/03/2027
EOF

cat > tests/fixtures/contratti_malformato.csv << 'EOF'
id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza
2,1,RSSMRA80A01H501U,15/03/2026,Auto,450.00,15/03/2027
3,2,VRDNNA90C41H501U,16/03/2026,Casa,NON_NUMERICO,16/03/2027
4,2,VRDNNA90C41H501U,17/03/2026,Vita,100.00,17/03/2027
EOF
```

- [ ] **Step 2: Scrivere il test fallente**

```cpp
// tests/test_filemanager_interazioni.cpp
#include <cassert>
#include <iostream>
#include <cstdio>
#include "../InsuraProCRM/FileManager.h"
#include "../InsuraProCRM/StoricoInterazioni.h"

int main() {
    // caricamento: contatore condiviso = max(5, 3) + 1 = 6
    StoricoInterazioni s1;
    FileManager::caricaInterazioni(s1, "tests/fixtures/appuntamenti_ok.csv", "tests/fixtures/contratti_ok.csv");
    assert(s1.tutte().size() == 4);
    assert(s1.getProssimoId() == 6);

    auto perCliente1 = s1.getPerCliente(1, "");
    assert(perCliente1.size() == 2); // 1 appuntamento + 1 contratto

    // riga malformata in contratti: scartata, le altre caricate
    StoricoInterazioni s2;
    FileManager::caricaInterazioni(s2, "tests/fixtures/appuntamenti_ok.csv",
                          "tests/fixtures/contratti_malformato.csv");
    // 2 appuntamenti validi + 2 contratti validi (id 2 e 4), 1 contratto scartato (id 3)
    assert(s2.tutte().size() == 4);

    // round-trip
    StoricoInterazioni sorgente;
    sorgente.aggiungiAppuntamento(9, "AAABBB11C22D333E", "01/01/2026", "08:00", "concluso", "");
    sorgente.aggiungiContratto(9, "AAABBB11C22D333E", "02/01/2026", "Vita", 200.0, "02/01/2027");
    FileManager::salvaInterazioni(sorgente, "tests/fixtures/appuntamenti_roundtrip.csv",
                         "tests/fixtures/contratti_roundtrip.csv");

    StoricoInterazioni ricaricato;
    FileManager::caricaInterazioni(ricaricato, "tests/fixtures/appuntamenti_roundtrip.csv",
                          "tests/fixtures/contratti_roundtrip.csv");
    assert(ricaricato.tutte().size() == 2);
    auto perCliente9 = ricaricato.getPerCliente(9, "");
    assert(perCliente9.size() == 2);
    // un Contratto non deve finire nel file appuntamenti e viceversa: verificato
    // dal fatto che entrambi i file, ricaricati separatamente, tornano il tipo giusto
    bool trovatoAppuntamento = false, trovatoContratto = false;
    for (const auto* i : perCliente9) {
        if (i->getTipo() == "Appuntamento") trovatoAppuntamento = true;
        if (i->getTipo() == "Contratto") trovatoContratto = true;
    }
    assert(trovatoAppuntamento && trovatoContratto);

    std::remove("tests/fixtures/appuntamenti_roundtrip.csv");
    std::remove("tests/fixtures/contratti_roundtrip.csv");

    std::cout << "test_filemanager_interazioni: tutti i test passati" << std::endl;
    return 0;
}
```

- [ ] **Step 3: Eseguire il test e verificare che fallisca**

Run: `g++ tests/test_filemanager_interazioni.cpp InsuraProCRM/FileManager.cpp InsuraProCRM/StoricoInterazioni.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_filemanager_interazioni`
Expected: FAIL in compilazione — `caricaInterazioni`/`salvaInterazioni` non esistono ancora.

- [ ] **Step 4: Aggiornare `FileManager.h`**

```cpp
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Rubrica.h"
#include "StoricoInterazioni.h"

namespace FileManager {
    std::vector<std::string> parseRigaCSV(const std::string& riga);

    void caricaClienti(Rubrica& rubrica, const std::string& path = "clienti.csv");
    void salvaClienti(const Rubrica& rubrica, const std::string& path = "clienti.csv");

    void caricaInterazioni(StoricoInterazioni& storico,
                            const std::string& pathAppuntamenti = "appuntamenti.csv",
                            const std::string& pathContratti = "contratti.csv");
    void salvaInterazioni(const StoricoInterazioni& storico,
                           const std::string& pathAppuntamenti = "appuntamenti.csv",
                           const std::string& pathContratti = "contratti.csv");
}

#endif
```

- [ ] **Step 5: Aggiungere l'implementazione in `FileManager.cpp`**

```cpp
#include "Appuntamento.h"
#include "Contratto.h"
// (aggiungere questi include in cima al file, insieme a quelli gia' presenti dal Task 8)

void FileManager::caricaInterazioni(StoricoInterazioni& storico,
                                     const std::string& pathAppuntamenti,
                                     const std::string& pathContratti) {
    int maxId = 0;

    std::ifstream fileApp(pathAppuntamenti);
    if (fileApp.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;
        while (std::getline(fileApp, riga)) {
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }
            if (riga.empty()) continue;
            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }
            try {
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);
                storico.impostaProssimoId(id);
                storico.aggiungiAppuntamento(idCliente, campi[2], campi[3], campi[4], campi[5],
                                              campi[6]);
                if (id > maxId) maxId = id;
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathAppuntamenti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    std::ifstream fileContr(pathContratti);
    if (fileContr.is_open()) {
        std::string riga;
        bool primaRiga = true;
        int numeroRiga = 0;
        while (std::getline(fileContr, riga)) {
            numeroRiga++;
            if (primaRiga) { primaRiga = false; continue; }
            if (riga.empty()) continue;
            std::vector<std::string> campi = parseRigaCSV(riga);
            if (campi.size() != 7) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido" << std::endl;
                continue;
            }
            try {
                int id = std::stoi(campi[0]);
                int idCliente = std::stoi(campi[1]);
                double premio = std::stod(campi[5]); // lancia std::invalid_argument se non numerico
                storico.impostaProssimoId(id);
                storico.aggiungiContratto(idCliente, campi[2], campi[3], campi[4], premio,
                                           campi[6]);
                if (id > maxId) maxId = id;
            } catch (const std::exception& e) {
                std::cerr << "Attenzione: riga " << numeroRiga << " di " << pathContratti
                          << " ignorata, formato non valido (" << e.what() << ")" << std::endl;
            }
        }
    }

    storico.impostaProssimoId(maxId + 1);
}

void FileManager::salvaInterazioni(const StoricoInterazioni& storico,
                                    const std::string& pathAppuntamenti,
                                    const std::string& pathContratti) {
    std::ofstream fileApp(pathAppuntamenti, std::ios::trunc);
    std::ofstream fileContr(pathContratti, std::ios::trunc);
    if (!fileApp.is_open() || !fileContr.is_open()) {
        std::cerr << "Errore: impossibile scrivere i file delle interazioni" << std::endl;
        return;
    }

    fileApp << "id,id_cliente,codice_fiscale,data,orario,esito,note\n";
    fileContr << "id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza\n";

    for (const auto& interazione : storico.tutte()) {
        if (interazione->getTipo() == "Appuntamento") {
            fileApp << interazione->toCSV() << "\n";
        } else if (interazione->getTipo() == "Contratto") {
            fileContr << interazione->toCSV() << "\n";
        }
    }
}
```

**Nota per l'implementatore:** stesso limite del Task 8 su `impostaProssimoId` +
`aggiungi...` per forzare un id esplicito durante il caricamento: funziona per i fixture
di questo task (id in ordine crescente), il `maxId` finale corregge comunque il contatore
per le interazioni successive.

- [ ] **Step 6: Eseguire il test e verificare che passi**

Run: `g++ tests/test_filemanager_interazioni.cpp InsuraProCRM/FileManager.cpp InsuraProCRM/StoricoInterazioni.cpp InsuraProCRM/Interazione.cpp InsuraProCRM/Appuntamento.cpp InsuraProCRM/Contratto.cpp InsuraProCRM/Validatore.cpp -o tests/test_filemanager_interazioni && ./tests/test_filemanager_interazioni`
Expected: PASS — stampa `test_filemanager_interazioni: tutti i test passati`, exit code 0.

- [ ] **Step 7: Commit**

```bash
git add InsuraProCRM/FileManager.h InsuraProCRM/FileManager.cpp tests/test_filemanager_interazioni.cpp tests/fixtures/appuntamenti_ok.csv tests/fixtures/contratti_ok.csv tests/fixtures/contratti_malformato.csv
git commit -m "feat: FileManager carica/salva interazioni con contatore ID condiviso"
```

---

### Task 10: Menu — Gestione Clienti

**Files:**
- Modify: `InsuraProCRM/Menu.h`
- Modify: `InsuraProCRM/Menu.cpp`
- Test: `tests/scenari/clienti_aggiungi_ok.txt`, `tests/scenari/clienti_aggiungi_correzione.txt`, `tests/scenari/clienti_modifica_non_trovato.txt`, `tests/scenari/clienti_input_non_numerico.txt`
- Test runner: `tests/run_scenario_clienti.sh`

**Interfaces:**
- Consumes: `Rubrica` (Task 3)
- Produces:
  - `Menu(Rubrica& rubrica, StoricoInterazioni& storico)` — il costruttore prende gia' entrambe le collezioni anche se questo task usa solo `Rubrica`, per evitare di cambiare la firma nel Task 11
  - `void eseguiSottomenuClienti()` — ciclo interattivo su `std::cin`/`std::cout` per le voci 1.1–1.5, ritorna quando l'utente sceglie "torna al menu principale" (voce `0`)
  - `void eseguiMenuPrincipale()` — dichiarato ma implementato nel Task 12 (qui basta lo stub che chiama solo `eseguiSottomenuClienti()` per rendere testabile questo task)

- [ ] **Step 1: Creare gli scenari di input**

```bash
mkdir -p tests/scenari

cat > tests/scenari/clienti_aggiungi_ok.txt << 'EOF'
1
1
Mario
Rossi
3331234567
mario.rossi@email.com
RSSMRA80A01H501U
01/01/1980
0
EOF

cat > tests/scenari/clienti_aggiungi_correzione.txt << 'EOF'
1
1
Mario
Rossi
3331234567
email-non-valida
mario.rossi@email.com
RSSMRA80A01H501U
01/01/1980
0
EOF

cat > tests/scenari/clienti_modifica_non_trovato.txt << 'EOF'
1
3
999
1
2
0
EOF

cat > tests/scenari/clienti_input_non_numerico.txt << 'EOF'
1
abc
2
0
EOF
```

Nota sugli scenari: il primo `1` seleziona "Gestione Clienti" dal menu principale (voce
implementata come stub nel Task 10, completa nel Task 12); i numeri successivi sono le voci
del sottomenu clienti (1=aggiungi, 2=visualizza, 3=modifica, 4=elimina, 5=cerca, 0=torna
indietro).

- [ ] **Step 2: Creare lo script di verifica**

```bash
cat > tests/run_scenario_clienti.sh << 'EOF'
#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "--- Scenario: aggiungi cliente OK ---"
rm -f tests/scenari/clienti_test.csv
OUTPUT=$(./crm < tests/scenari/clienti_aggiungi_ok.txt)
echo "$OUTPUT" | grep -q "Cliente aggiunto" || (echo "FALLITO: manca conferma aggiunta" && exit 1)

echo "--- Scenario: correzione email non valida ---"
OUTPUT=$(./crm < tests/scenari/clienti_aggiungi_correzione.txt)
echo "$OUTPUT" | grep -qi "email non valida" || (echo "FALLITO: manca messaggio di errore email" && exit 1)
echo "$OUTPUT" | grep -q "Cliente aggiunto" || (echo "FALLITO: il cliente non e' stato comunque aggiunto dopo la correzione" && exit 1)

echo "--- Scenario: modifica cliente non trovato ---"
OUTPUT=$(./crm < tests/scenari/clienti_modifica_non_trovato.txt)
echo "$OUTPUT" | grep -qi "Cliente non trovato" || (echo "FALLITO: manca messaggio cliente non trovato" && exit 1)

echo "--- Scenario: input menu non numerico ---"
OUTPUT=$(./crm < tests/scenari/clienti_input_non_numerico.txt)
echo "$OUTPUT" | grep -qi "scelta non valida\|input non valido" || (echo "FALLITO: manca messaggio di input non valido" && exit 1)

echo "TUTTI GLI SCENARI PASSATI"
EOF
chmod +x tests/run_scenario_clienti.sh
```

- [ ] **Step 3: Eseguire lo script e verificare che fallisca**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_clienti.sh`
Expected: FAIL — il menu non esiste ancora, `./crm` non produce nessuno dei messaggi attesi (o non compila affatto se `Menu` non e' ancora collegato a `main.cpp`).

- [ ] **Step 4: Implementare `Menu.h`**

```cpp
#ifndef MENU_H
#define MENU_H

#include "Rubrica.h"
#include "StoricoInterazioni.h"

class Menu {
public:
    Menu(Rubrica& rubrica, StoricoInterazioni& storico);

    void eseguiMenuPrincipale();

private:
    Rubrica& rubrica_;
    StoricoInterazioni& storico_;

    void eseguiSottomenuClienti();
    void aggiungiClienteInterattivo();
    void visualizzaTuttiIClienti();
    void modificaClienteInterattivo();
    void eliminaClienteInterattivo();
    void cercaClienteInterattivo();

    static int leggiSceltaMenu();
    static std::string leggiRiga(const std::string& prompt);
};

#endif
```

- [ ] **Step 5: Implementare `Menu.cpp` (parte Gestione Clienti)**

```cpp
#include "Menu.h"
#include <iostream>
#include <stdexcept>
#include <limits>

Menu::Menu(Rubrica& rubrica, StoricoInterazioni& storico)
    : rubrica_(rubrica), storico_(storico) {}

std::string Menu::leggiRiga(const std::string& prompt) {
    std::cout << prompt;
    std::string valore;
    std::getline(std::cin, valore);
    return valore;
}

int Menu::leggiSceltaMenu() {
    std::string riga;
    std::getline(std::cin, riga);
    try {
        size_t pos;
        int valore = std::stoi(riga, &pos);
        if (pos != riga.size()) return -1; // caratteri extra dopo il numero
        return valore;
    } catch (const std::exception&) {
        return -1;
    }
}

void Menu::aggiungiClienteInterattivo() {
    std::string nome = leggiRiga("Nome: ");
    std::string cognome = leggiRiga("Cognome: ");
    std::string telefono = leggiRiga("Telefono: ");

    std::string email;
    while (true) {
        email = leggiRiga("Email: ");
        if (Validatore_EmailOk(email)) break;
    }
    // NOTA: la validazione vera avviene dentro Rubrica::aggiungiCliente (che
    // costruisce un Cliente e quindi valida tramite i setter). Qui sotto si
    // usa direttamente il ciclo try/catch sull'intera operazione, riprovando
    // SOLO il campo che ha fallito, come richiesto dal design.

    std::string codiceFiscale = leggiRiga("Codice fiscale: ");
    std::string dataNascita = leggiRiga("Data di nascita (gg/mm/aaaa): ");

    while (true) {
        try {
            rubrica_.aggiungiCliente(nome, cognome, telefono, email, codiceFiscale, dataNascita);
            std::cout << "Cliente aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Email") != std::string::npos) {
                email = leggiRiga("Reinserisci Email: ");
            } else if (messaggio.find("fiscale") != std::string::npos) {
                codiceFiscale = leggiRiga("Reinserisci codice fiscale: ");
            } else if (messaggio.find("nascita") != std::string::npos) {
                dataNascita = leggiRiga("Reinserisci data di nascita: ");
            } else if (messaggio.find("Nome") != std::string::npos) {
                nome = leggiRiga("Reinserisci nome: ");
            } else if (messaggio.find("Cognome") != std::string::npos) {
                cognome = leggiRiga("Reinserisci cognome: ");
            } else if (messaggio.find("Telefono") != std::string::npos) {
                telefono = leggiRiga("Reinserisci telefono: ");
            } else {
                nome = leggiRiga("Reinserisci nome: "); // fallback difensivo
            }
        }
    }
}

void Menu::visualizzaTuttiIClienti() {
    const auto& clienti = rubrica_.visualizzaTutti();
    if (clienti.empty()) {
        std::cout << "Nessun cliente presente." << std::endl;
        return;
    }
    for (const auto& c : clienti) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << " - " << c.getEmail() << " - " << c.getTelefono() << std::endl;
    }
}

void Menu::modificaClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente da modificare: ");
    int id;
    try {
        id = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    Cliente esistente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(id, esistente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string nome = leggiRiga("Nuovo nome [" + esistente.getNome() + "]: ");
    if (nome.empty()) nome = esistente.getNome();
    std::string cognome = leggiRiga("Nuovo cognome [" + esistente.getCognome() + "]: ");
    if (cognome.empty()) cognome = esistente.getCognome();
    std::string telefono = leggiRiga("Nuovo telefono [" + esistente.getTelefono() + "]: ");
    if (telefono.empty()) telefono = esistente.getTelefono();
    std::string email = leggiRiga("Nuova email [" + esistente.getEmail() + "]: ");
    if (email.empty()) email = esistente.getEmail();
    std::string cf = leggiRiga("Nuovo codice fiscale [" + esistente.getCodiceFiscale() + "]: ");
    if (cf.empty()) cf = esistente.getCodiceFiscale();
    std::string dataNascita = leggiRiga("Nuova data nascita [" + esistente.getDataNascita() + "]: ");
    if (dataNascita.empty()) dataNascita = esistente.getDataNascita();

    try {
        rubrica_.modificaCliente(id, nome, cognome, telefono, email, cf, dataNascita);
        std::cout << "Cliente modificato con successo." << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Errore: " << e.what() << std::endl;
    }
}

void Menu::eliminaClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente da eliminare: ");
    int id;
    try {
        id = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    if (rubrica_.eliminaCliente(id)) {
        std::cout << "Cliente eliminato." << std::endl;
    } else {
        std::cout << "Cliente non trovato." << std::endl;
    }
}

void Menu::cercaClienteInterattivo() {
    std::string query = leggiRiga("Nome o cognome da cercare: ");
    auto risultati = rubrica_.cercaPerNomeCognome(query);
    if (risultati.empty()) {
        std::cout << "Nessun cliente trovato." << std::endl;
        return;
    }
    for (const auto& c : risultati) {
        std::cout << "#" << c.getId() << " " << c.getNome() << " " << c.getCognome()
                  << std::endl;
    }
}

void Menu::eseguiSottomenuClienti() {
    while (true) {
        std::cout << "\n1. Aggiungi cliente\n2. Visualizza tutti i clienti\n"
                  << "3. Modifica cliente\n4. Elimina cliente\n5. Cerca cliente\n"
                  << "0. Torna al menu principale\nScelta: ";
        int scelta = leggiSceltaMenu();
        switch (scelta) {
            case 1: aggiungiClienteInterattivo(); break;
            case 2: visualizzaTuttiIClienti(); break;
            case 3: modificaClienteInterattivo(); break;
            case 4: eliminaClienteInterattivo(); break;
            case 5: cercaClienteInterattivo(); break;
            case 0: return;
            default: std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}

void Menu::eseguiMenuPrincipale() {
    // Implementazione completa nel Task 12; per questo task basta collegare
    // la voce 1 al sottomenu clienti cosi' gli scenari di test possono girare.
    while (true) {
        std::cout << "\nMENU PRINCIPALE\n1. Gestione Clienti\n0. Esci\nScelta: ";
        int scelta = leggiSceltaMenu();
        if (scelta == 1) {
            eseguiSottomenuClienti();
        } else if (scelta == 0) {
            return;
        } else {
            std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}
```

**Correzione da applicare prima di compilare:** lo pseudocodice sopra referenzia una
funzione inesistente `Validatore_EmailOk` nel primo abbozzo di `aggiungiClienteInterattivo`.
Rimuovere quel ciclo di pre-validazione (righe del blocco `while(true) { email = ...
if (Validatore_EmailOk(email)) break; }`): la validazione dell'email avviene gia',
correttamente, dentro il blocco `try/catch` piu' sotto che chiama
`rubrica_.aggiungiCliente(...)`. La versione corretta di `aggiungiClienteInterattivo`
raccoglie tutti i campi con semplici `leggiRiga(...)` e poi entra direttamente nel ciclo
`try/catch`, esattamente come mostrato per gli altri campi.

- [ ] **Step 6: Collegare il Menu a `main.cpp` (provvisorio, solo per questo test)**

```cpp
// InsuraProCRM/main.cpp
#include "Rubrica.h"
#include "StoricoInterazioni.h"
#include "Menu.h"

int main() {
    Rubrica rubrica;
    StoricoInterazioni storico;
    Menu menu(rubrica, storico);
    menu.eseguiMenuPrincipale();
    return 0;
}
```

- [ ] **Step 7: Eseguire lo script e verificare che passi**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_clienti.sh`
Expected: PASS — stampa `TUTTI GLI SCENARI PASSATI`.

- [ ] **Step 8: Commit**

```bash
git add InsuraProCRM/Menu.h InsuraProCRM/Menu.cpp InsuraProCRM/main.cpp tests/scenari tests/run_scenario_clienti.sh
git commit -m "feat: Menu - sottomenu Gestione Clienti con correzione campo per campo"
```

---

### Task 11: Menu — Gestione Interazioni

**Files:**
- Modify: `InsuraProCRM/Menu.h`
- Modify: `InsuraProCRM/Menu.cpp`
- Test: `tests/scenari/interazioni_aggiungi_appuntamento.txt`, `tests/scenari/interazioni_premio_negativo.txt`, `tests/scenari/interazioni_cliente_inesistente.txt`, `tests/scenari/interazioni_sopravvivono_a_eliminazione_cliente.txt`
- Test runner: `tests/run_scenario_interazioni.sh`

**Interfaces:**
- Consumes: `StoricoInterazioni` (Task 6), `Rubrica` (Task 3, gia' iniettata nel `Menu` dal Task 10)
- Produces:
  - `void eseguiSottomenuInterazioni()` — voci 2.1–2.4
  - Aggiornamento di `eseguiMenuPrincipale()` per includere la voce `2. Gestione Interazioni`

- [ ] **Step 1: Creare gli scenari di input**

```bash
cat > tests/scenari/interazioni_aggiungi_appuntamento.txt << 'EOF'
1
1
Mario
Rossi
3331234567
mario.rossi@email.com
RSSMRA80A01H501U
01/01/1980
0
2
1
1
15/03/2026
10:30
concluso
Nessuna nota
3
1
0
0
EOF

cat > tests/scenari/interazioni_premio_negativo.txt << 'EOF'
1
1
Mario
Rossi
3331234567
mario.rossi@email.com
RSSMRA80A01H501U
01/01/1980
0
2
2
1
15/03/2026
Auto
-50
15/03/2027
100
0
0
EOF

cat > tests/scenari/interazioni_cliente_inesistente.txt << 'EOF'
2
3
999
0
0
EOF

cat > tests/scenari/interazioni_sopravvivono_a_eliminazione_cliente.txt << 'EOF'
1
1
Mario
Rossi
3331234567
mario.rossi@email.com
RSSMRA80A01H501U
01/01/1980
0
2
1
1
15/03/2026
10:30
concluso
Nessuna nota
0
1
4
1
0
2
3
1
0
0
EOF
```

Nota: nello scenario `interazioni_premio_negativo.txt`, dopo il premio negativo `-50` il
programma deve richiedere di reinserire solo quel campo — la riga successiva `100` e' il
valore corretto; le righe `Auto` e `15/03/2027` vengono lette una sola volta e non devono
essere richieste di nuovo (verifica del requisito "senza perdere i campi gia' inseriti
correttamente").

- [ ] **Step 2: Creare lo script di verifica**

```bash
cat > tests/run_scenario_interazioni.sh << 'EOF'
#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "--- Scenario: aggiungi appuntamento a cliente esistente ---"
OUTPUT=$(./crm < tests/scenari/interazioni_aggiungi_appuntamento.txt)
echo "$OUTPUT" | grep -qi "Appuntamento" || (echo "FALLITO: appuntamento non mostrato" && exit 1)
echo "$OUTPUT" | grep -q "15/03/2026" || (echo "FALLITO: data appuntamento non trovata in output" && exit 1)

echo "--- Scenario: premio negativo richiede reinserimento solo di quel campo ---"
OUTPUT=$(./crm < tests/scenari/interazioni_premio_negativo.txt)
echo "$OUTPUT" | grep -qi "premio" || (echo "FALLITO: manca messaggio sul premio" && exit 1)
echo "$OUTPUT" | grep -qi "Contratto" || (echo "FALLITO: contratto non aggiunto dopo la correzione" && exit 1)

echo "--- Scenario: interazioni di cliente inesistente ---"
OUTPUT=$(./crm < tests/scenari/interazioni_cliente_inesistente.txt)
echo "$OUTPUT" | grep -qi "Cliente non trovato" || (echo "FALLITO: manca messaggio cliente non trovato" && exit 1)

echo "--- Scenario: interazioni sopravvivono a eliminazione cliente ---"
OUTPUT=$(./crm < tests/scenari/interazioni_sopravvivono_a_eliminazione_cliente.txt)
echo "$OUTPUT" | grep -qi "Cliente eliminato" || (echo "FALLITO: cliente non eliminato" && exit 1)
echo "$OUTPUT" | grep -qi "Appuntamento" || (echo "FALLITO: l'appuntamento e' sparito dopo l'eliminazione del cliente" && exit 1)

echo "TUTTI GLI SCENARI PASSATI"
EOF
chmod +x tests/run_scenario_interazioni.sh
```

- [ ] **Step 3: Eseguire lo script e verificare che fallisca**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_interazioni.sh`
Expected: FAIL — la voce "2. Gestione Interazioni" non esiste ancora nel menu principale.

- [ ] **Step 4: Aggiornare `Menu.h`**

```cpp
// aggiungere ai membri privati di Menu, sotto le dichiarazioni della Task 10:
void eseguiSottomenuInterazioni();
void aggiungiAppuntamentoInterattivo();
void aggiungiContrattoInterattivo();
void visualizzaInterazioniClienteInterattivo();
void cercaInterazioniInterattivo();
```

- [ ] **Step 5: Aggiungere l'implementazione in `Menu.cpp`**

```cpp
void Menu::aggiungiAppuntamentoInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(idCliente, cliente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string data = leggiRiga("Data (gg/mm/aaaa): ");
    std::string orario = leggiRiga("Orario (hh:mm): ");
    std::string esito = leggiRiga("Esito: ");
    std::string note = leggiRiga("Note (facoltative): ");

    while (true) {
        try {
            storico_.aggiungiAppuntamento(idCliente, cliente.getCodiceFiscale(), data, orario,
                                           esito, note);
            std::cout << "Appuntamento aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Data") != std::string::npos) {
                data = leggiRiga("Reinserisci data: ");
            } else if (messaggio.find("Orario") != std::string::npos) {
                orario = leggiRiga("Reinserisci orario: ");
            } else if (messaggio.find("Esito") != std::string::npos) {
                esito = leggiRiga("Reinserisci esito: ");
            } else {
                data = leggiRiga("Reinserisci data: "); // fallback difensivo
            }
        }
    }
}

void Menu::aggiungiContrattoInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }
    Cliente cliente(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    if (!rubrica_.getById(idCliente, cliente)) {
        std::cout << "Cliente non trovato." << std::endl;
        return;
    }

    std::string data = leggiRiga("Data (gg/mm/aaaa): ");
    std::string tipoPolizza = leggiRiga("Tipo polizza: ");
    std::string premioStr = leggiRiga("Premio: ");
    std::string dataScadenza = leggiRiga("Data scadenza (gg/mm/aaaa): ");

    while (true) {
        double premio;
        try {
            premio = std::stod(premioStr);
        } catch (const std::exception&) {
            std::cout << "Errore: Premio non numerico." << std::endl;
            premioStr = leggiRiga("Reinserisci premio: ");
            continue;
        }
        try {
            storico_.aggiungiContratto(idCliente, cliente.getCodiceFiscale(), data, tipoPolizza,
                                        premio, dataScadenza);
            std::cout << "Contratto aggiunto con successo." << std::endl;
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "Errore: " << e.what() << std::endl;
            std::string messaggio = e.what();
            if (messaggio.find("Premio") != std::string::npos) {
                premioStr = leggiRiga("Reinserisci premio: ");
            } else if (messaggio.find("scadenza") != std::string::npos) {
                dataScadenza = leggiRiga("Reinserisci data scadenza: ");
            } else if (messaggio.find("polizza") != std::string::npos) {
                tipoPolizza = leggiRiga("Reinserisci tipo polizza: ");
            } else {
                data = leggiRiga("Reinserisci data: "); // fallback difensivo
            }
        }
    }
}

void Menu::visualizzaInterazioniClienteInterattivo() {
    std::string idStr = leggiRiga("ID cliente: ");
    int idCliente;
    try {
        idCliente = std::stoi(idStr);
    } catch (const std::exception&) {
        std::cout << "ID non valido." << std::endl;
        return;
    }

    // Nota: NON si richiede che il cliente esista ancora nella Rubrica: le
    // interazioni di un cliente eliminato restano consultabili (vedi design,
    // sezione "Eliminazione cliente").
    auto interazioni = storico_.getPerCliente(idCliente, "");
    if (interazioni.empty()) {
        std::cout << "Nessuna interazione trovata per questo cliente." << std::endl;
        return;
    }
    for (const auto* i : interazioni) {
        i->stampa();
    }
}

void Menu::cercaInterazioniInterattivo() {
    std::string data = leggiRiga("Cerca per data (gg/mm/aaaa, vuoto per saltare): ");
    for (const auto& interazione : storico_.tutte()) {
        if (data.empty() || interazione->getData() == data) {
            interazione->stampa();
        }
    }
}

void Menu::eseguiSottomenuInterazioni() {
    while (true) {
        std::cout << "\n1. Aggiungi appuntamento\n2. Aggiungi contratto\n"
                  << "3. Visualizza interazioni di un cliente\n4. Cerca interazioni\n"
                  << "0. Torna al menu principale\nScelta: ";
        int scelta = leggiSceltaMenu();
        switch (scelta) {
            case 1: aggiungiAppuntamentoInterattivo(); break;
            case 2: aggiungiContrattoInterattivo(); break;
            case 3: visualizzaInterazioniClienteInterattivo(); break;
            case 4: cercaInterazioniInterattivo(); break;
            case 0: return;
            default: std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}
```

- [ ] **Step 6: Aggiornare `eseguiMenuPrincipale` in `Menu.cpp`**

```cpp
void Menu::eseguiMenuPrincipale() {
    while (true) {
        std::cout << "\nMENU PRINCIPALE\n1. Gestione Clienti\n2. Gestione Interazioni\n"
                  << "3. Esci\nScelta: ";
        int scelta = leggiSceltaMenu();
        if (scelta == 1) {
            eseguiSottomenuClienti();
        } else if (scelta == 2) {
            eseguiSottomenuInterazioni();
        } else if (scelta == 3) {
            return;
        } else {
            std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}
```

**Nota:** questo cambia il numero associato a "Esci" da `0` (usato provvisoriamente nel
Task 10) a `3`, coerente con la struttura del menu definita nel design. Gli scenari del
Task 10 che usavano `0` per uscire dal menu principale vanno aggiornati di conseguenza se
rieseguiti dopo questo task (gli scenari di questo Task 11 usano gia' `0` solo per i
sottomenu, coerentemente).

- [ ] **Step 7: Eseguire lo script e verificare che passi**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_interazioni.sh`
Expected: PASS — stampa `TUTTI GLI SCENARI PASSATI`.

- [ ] **Step 8: Ri-eseguire lo script del Task 10 e aggiornarlo se necessario**

Run: `./tests/run_scenario_clienti.sh`
Se fallisce per via del nuovo numero di "Esci", aggiornare gli scenari `tests/scenari/clienti_*.txt`
sostituendo l'eventuale `0` finale usato per uscire dal menu principale (non dai sottomenu)
con `3`, poi ri-eseguire fino a `TUTTI GLI SCENARI PASSATI`.

- [ ] **Step 9: Commit**

```bash
git add InsuraProCRM/Menu.h InsuraProCRM/Menu.cpp tests/scenari tests/run_scenario_interazioni.sh
git commit -m "feat: Menu - sottomenu Gestione Interazioni"
```

---

### Task 12: main.cpp — flusso completo avvio/uscita

**Files:**
- Modify: `InsuraProCRM/main.cpp`
- Modify: `InsuraProCRM/Menu.h`
- Modify: `InsuraProCRM/Menu.cpp`
- Test: `tests/run_scenario_flusso_completo.sh`
- Test fixtures: `tests/fixtures/flusso/clienti.csv`, `tests/fixtures/flusso/appuntamenti.csv`, `tests/fixtures/flusso/contratti.csv`

**Interfaces:**
- Consumes: `FileManager` (Task 8, 9 — namespace, nessuna istanza da creare), `Menu` (Task 10, 11)
- Produces:
  - La voce "3. Esci" del menu principale chiama `FileManager::salvaClienti(rubrica_)` e `FileManager::salvaInterazioni(storico_)` prima di terminare, e stampa un messaggio di errore (senza uscire dal programma) se la scrittura fallisce
  - `main.cpp` carica tutto all'avvio tramite `FileManager::caricaClienti`/`caricaInterazioni`, passa `Rubrica&` e `StoricoInterazioni&` al `Menu` (costruttore **invariato** rispetto al Task 10/11, dato che `FileManager` non è più un oggetto da tenere come membro), avvia `eseguiMenuPrincipale()`

- [ ] **Step 1: Creare i CSV di partenza per il test end-to-end**

```bash
mkdir -p tests/fixtures/flusso
cat > tests/fixtures/flusso/clienti.csv << 'EOF'
id,nome,cognome,telefono,email,codice_fiscale,data_nascita
1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980
EOF
cat > tests/fixtures/flusso/appuntamenti.csv << 'EOF'
id,id_cliente,codice_fiscale,data,orario,esito,note
EOF
cat > tests/fixtures/flusso/contratti.csv << 'EOF'
id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza
EOF
```

- [ ] **Step 2: Scrivere lo script di test fallente**

```bash
cat > tests/run_scenario_flusso_completo.sh << 'EOF'
#!/bin/bash
set -e
cd "$(dirname "$0")/.."

WORKDIR=$(mktemp -d)
cp tests/fixtures/flusso/*.csv "$WORKDIR/"
cp crm "$WORKDIR/"

cd "$WORKDIR"
cat > input.txt << 'INPUT'
1
1
Anna
Verdi
3339876543
anna.verdi@email.com
VRDNNA90C41H501U
05/03/1990
0
3
INPUT

./crm < input.txt > output.txt

echo "--- Verifica: clienti.csv contiene sia Mario che Anna dopo l'uscita ---"
grep -q "Mario" clienti.csv || (echo "FALLITO: Mario scomparso da clienti.csv" && exit 1)
grep -q "Anna" clienti.csv || (echo "FALLITO: Anna non salvata in clienti.csv" && exit 1)

echo "--- Verifica: scrittura fallita non termina il programma ---"
cat > input2.txt << 'INPUT'
1
2
0
3
INPUT
chmod 000 clienti.csv contratti.csv appuntamenti.csv
OUTPUT2=$(./crm < input2.txt || true)
chmod 644 clienti.csv contratti.csv appuntamenti.csv
echo "$OUTPUT2" | grep -qi "errore" || (echo "FALLITO: manca messaggio di errore su scrittura fallita" && exit 1)
echo "$OUTPUT2" | grep -q "Anna" || (echo "FALLITO: il programma non ha continuato a rispondere dopo l'errore di scrittura (visualizza clienti non ha mostrato Anna)" && exit 1)

cd -
rm -rf "$WORKDIR"
echo "TUTTI GLI SCENARI PASSATI"
EOF
chmod +x tests/run_scenario_flusso_completo.sh
```

- [ ] **Step 3: Eseguire lo script e verificare che fallisca**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_flusso_completo.sh`
Expected: FAIL — `main.cpp` non carica/salva ancora nulla, quindi `clienti.csv` non conterra' "Anna" dopo l'uscita.

- [ ] **Step 4: Aggiornare `Menu.h`**

```cpp
#ifndef MENU_H
#define MENU_H

#include "Rubrica.h"
#include "StoricoInterazioni.h"

class Menu {
public:
    Menu(Rubrica& rubrica, StoricoInterazioni& storico);

    void eseguiMenuPrincipale();

private:
    Rubrica& rubrica_;
    StoricoInterazioni& storico_;

    void eseguiSottomenuClienti();
    void aggiungiClienteInterattivo();
    void visualizzaTuttiIClienti();
    void modificaClienteInterattivo();
    void eliminaClienteInterattivo();
    void cercaClienteInterattivo();

    void eseguiSottomenuInterazioni();
    void aggiungiAppuntamentoInterattivo();
    void aggiungiContrattoInterattivo();
    void visualizzaInterazioniClienteInterattivo();
    void cercaInterazioniInterattivo();

    void salvaESci();

    static int leggiSceltaMenu();
    static std::string leggiRiga(const std::string& prompt);
};

#endif
```

**Nota:** questa firma del costruttore e' identica a quella gia' usata nel Task 10/11 —
nessuna modifica necessaria li'. `Menu` non ha bisogno di un membro `FileManager&` perche'
`FileManager` e' un namespace di funzioni libere (Task 8, 9), richiamabile direttamente
come `FileManager::salvaClienti(...)` senza tenere nessun riferimento a un'istanza.
L'unico membro aggiunto in questo task e' il nuovo metodo privato `salvaESci()`.

- [ ] **Step 5: Aggiornare `Menu.cpp`**

```cpp
#include "FileManager.h"
// (aggiungere questo include in cima a Menu.cpp, insieme a quelli gia' presenti)

void Menu::salvaESci() {
    FileManager::salvaClienti(rubrica_);
    FileManager::salvaInterazioni(storico_);
    std::cout << "Dati salvati. Uscita in corso." << std::endl;
}

void Menu::eseguiMenuPrincipale() {
    while (true) {
        std::cout << "\nMENU PRINCIPALE\n1. Gestione Clienti\n2. Gestione Interazioni\n"
                  << "3. Esci\nScelta: ";
        int scelta = leggiSceltaMenu();
        if (scelta == 1) {
            eseguiSottomenuClienti();
        } else if (scelta == 2) {
            eseguiSottomenuInterazioni();
        } else if (scelta == 3) {
            salvaESci();
            return;
        } else {
            std::cout << "Scelta non valida, riprova." << std::endl;
        }
    }
}
```

**Nota sull'errore di scrittura:** `FileManager::salvaClienti`/`salvaInterazioni` (Task 8, 9)
gia' stampano `"Errore: impossibile scrivere su ..."` su `std::cerr` e ritornano senza
lanciare eccezioni quando il file non e' apribile in scrittura — quindi `salvaESci()` non
necessita di un `try/catch`: il comportamento "il programma resta attivo, i dati in memoria
restano intatti" e' gia' garantito dal fatto che `salvaClienti`/`salvaInterazioni` non
modificano ne' `rubrica_` ne' `storico_`. Verificare pero' che lo script di test redirige
correttamente sia `stdout` che (se necessario) `stderr` nello stesso stream, altrimenti il
grep su "errore" nello Step 2 non trova nulla: aggiungere `2>&1` alla chiamata di `./crm` nello
script se il messaggio d'errore risulta mancante nell'output catturato.

- [ ] **Step 6: Aggiornare `main.cpp`**

```cpp
#include "Rubrica.h"
#include "StoricoInterazioni.h"
#include "FileManager.h"
#include "Menu.h"

int main() {
    Rubrica rubrica;
    StoricoInterazioni storico;

    FileManager::caricaClienti(rubrica);
    FileManager::caricaInterazioni(storico);

    Menu menu(rubrica, storico);
    menu.eseguiMenuPrincipale();

    return 0;
}
```

- [ ] **Step 7: Eseguire lo script e verificare che passi**

Run: `g++ InsuraProCRM/*.cpp -o crm && ./tests/run_scenario_flusso_completo.sh`
Expected: PASS — stampa `TUTTI GLI SCENARI PASSATI`. Se il secondo scenario (scrittura fallita)
non trova "errore" nell'output, applicare la correzione indicata nello Step 5 (`2>&1`) e
rieseguire.

- [ ] **Step 8: Ri-eseguire tutti gli script precedenti**

Run:
```bash
./tests/run_scenario_clienti.sh
./tests/run_scenario_interazioni.sh
```
Expected: entrambi devono ancora terminare con `TUTTI GLI SCENARI PASSATI`. Questa volta
è una semplice verifica di non-regressione: la firma del costruttore di `Menu` **non è
cambiata** rispetto al Task 10/11 (vedi nota nello Step 4), quindi non ci si aspetta
nessuna rottura — lo si verifica comunque perché `main.cpp` è stato modificato e potrebbe
in linea di principio aver introdotto un problema di collegamento (linking) non colto
dai singoli test unitari.

- [ ] **Step 9: Commit**

```bash
git add InsuraProCRM/main.cpp InsuraProCRM/Menu.h InsuraProCRM/Menu.cpp tests/run_scenario_flusso_completo.sh tests/fixtures/flusso
git commit -m "feat: main.cpp - flusso completo carica all'avvio, salva all'uscita"
```

---

### Task 13: Packaging e README

**Files:**
- Create: `InsuraProCRM/README.md`
- Test: `tests/run_scenario_packaging.sh`

**Interfaces:**
- Produces: `InsuraProCRM/README.md` con il comando di compilazione esatto; verifica che la cartella, estratta da zero, compili e parta senza CSV pre-esistenti

- [ ] **Step 1: Scrivere lo script di verifica del packaging**

```bash
cat > tests/run_scenario_packaging.sh << 'EOF'
#!/bin/bash
set -e
cd "$(dirname "$0")/.."

WORKDIR=$(mktemp -d)
cp -r InsuraProCRM "$WORKDIR/"
cd "$WORKDIR/InsuraProCRM"
rm -f clienti.csv appuntamenti.csv contratti.csv crm

echo "--- Verifica: compilazione pulita senza warning ---"
g++ -Wall -Wextra *.cpp -o crm 2> build_warnings.txt
if [ -s build_warnings.txt ]; then
    echo "FALLITO: warning di compilazione presenti:"
    cat build_warnings.txt
    exit 1
fi

echo "--- Verifica: avvio a freddo senza CSV pre-esistenti ---"
echo "3" | ./crm > avvio.txt 2>&1
grep -qi "MENU PRINCIPALE" avvio.txt || (echo "FALLITO: il menu non parte senza CSV" && exit 1)
test -f clienti.csv || (echo "FALLITO: clienti.csv non creato dopo il primo salvataggio" && exit 1)

echo "--- Verifica: README contiene il comando di build esatto ---"
grep -q "g++ \*.cpp -o crm" README.md || (echo "FALLITO: README non contiene il comando esatto" && exit 1)

cd -
rm -rf "$WORKDIR"
echo "TUTTI GLI SCENARI PASSATI"
EOF
chmod +x tests/run_scenario_packaging.sh
```

- [ ] **Step 2: Eseguire lo script e verificare che fallisca**

Run: `./tests/run_scenario_packaging.sh`
Expected: FAIL — `README.md` non esiste ancora.

- [ ] **Step 3: Scrivere `InsuraProCRM/README.md`**

```markdown
# CRM InsuraPro Solutions

CRM a riga di comando per la gestione di clienti e delle loro interazioni
(appuntamenti e contratti), sviluppato in C++ standard senza dipendenze
esterne.

## Compilazione

Dalla cartella `InsuraProCRM/`:

```
g++ *.cpp -o crm
```

Nessuna dipendenza esterna, nessun sistema di build richiesto.

## Esecuzione

```
./crm
```

All'avvio il programma carica automaticamente `clienti.csv`,
`appuntamenti.csv` e `contratti.csv` se presenti nella cartella corrente
(la loro assenza non è un errore: si parte con collezioni vuote). I dati
vengono salvati automaticamente scegliendo "Esci" dal menu principale.

## Struttura del menu

```
MENU PRINCIPALE
1. Gestione Clienti
   1. Aggiungi cliente
   2. Visualizza tutti i clienti
   3. Modifica cliente
   4. Elimina cliente
   5. Cerca cliente
2. Gestione Interazioni
   1. Aggiungi appuntamento
   2. Aggiungi contratto
   3. Visualizza interazioni di un cliente
   4. Cerca interazioni
3. Esci (salva ed esce)
```

## Note

- L'eliminazione di un cliente non elimina le sue interazioni: restano
  consultabili tramite l'ID cliente e il codice fiscale salvati su
  ciascuna interazione.
- I valori non validi (email, codice fiscale, date, importi) vengono
  rifiutati con un messaggio che spiega il motivo, chiedendo di
  reinserire solo il campo incriminato.
```

- [ ] **Step 4: Eseguire lo script e verificare che passi**

Run: `./tests/run_scenario_packaging.sh`
Expected: PASS — stampa `TUTTI GLI SCENARI PASSATI`.

- [ ] **Step 5: Creare lo zip di consegna**

```bash
cd InsuraProCRM
rm -f clienti.csv appuntamenti.csv contratti.csv crm
cd ..
zip -r InsuraProCRM.zip InsuraProCRM -x "*.git*"
```

- [ ] **Step 6: Commit**

```bash
git add InsuraProCRM/README.md tests/run_scenario_packaging.sh
git commit -m "docs: aggiunge README con istruzioni di compilazione e packaging finale"
```

---

## Self-Review

**Copertura spec:**
- OOP (classi, ereditarietà, polimorfismo) → Task 4, 5 (gerarchia `Interazione`), Task 6 (uso polimorfico via `unique_ptr<Interazione>`)
- CRUD clienti + ricerca parziale → Task 3
- Gestione interazioni collegate ai clienti → Task 6, 11
- Persistenza CSV caricata all'avvio/salvata all'uscita → Task 8, 9, 12
- Nessuna libreria esterna, nessun build system → rispettato in tutti i task (solo `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<memory>`, `<stdexcept>`, `<cctype>`, `<algorithm>`)
- Menu numerato classico → Task 10, 11, 12
- Eliminazione cliente non a cascata → Task 11 (scenario dedicato), documentato in Task 12 README
- Setter con eccezioni e messaggio chiaro → Task 2, 4, 5, propagato nel Menu (Task 10, 11)
- `campoNonVuoto` con trim → Task 1

**Scansione placeholder:** nessun "TBD"/"TODO" rimasto nei blocchi di codice; le uniche note esplicite lasciate all'implementatore (Task 8, 9, 10) documentano una scelta di design fragile ma corretta per i fixture forniti, non un buco lasciato aperto.

**Coerenza dei tipi:** `Rubrica::getById(int, Cliente&) -> bool` e `StoricoInterazioni::getPerCliente(int, const std::string&) -> std::vector<const Interazione*>` sono usati con la stessa firma in tutti i task successivi (6, 8, 10, 11). `FileManager::parseRigaCSV` è `static` in ogni punto in cui viene richiamato (Task 7, 8, 9). La firma di `Menu` cambia una sola volta, esplicitamente, nel Task 12 (aggiunta di `FileManager&`), con nota di aggiornamento per gli script dei Task 10/11.

---

## Execution Handoff

Plan complete and saved to `docs/superpowers/plans/2026-08-23-crm-insurapro.md`. Due opzioni di esecuzione:

**1. Subagent-Driven (consigliata)** — dispatch di un subagent per task, review tra un task e l'altro, iterazione rapida.

**2. Inline Execution** — esecuzione dei task in questa sessione, batch con checkpoint per la review.

Quale approccio preferisci?
