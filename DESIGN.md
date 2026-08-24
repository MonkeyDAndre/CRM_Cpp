# Design: CRM InsuraPro Solutions (C++)

Data: 2026-08-23

## Contesto e obiettivo

InsuraPro Solutions necessita di un CRM a riga di comando per gestire
clienti e le loro interazioni (appuntamenti e contratti). Il progetto
va consegnato come file zip contenente i sorgenti C++, compilabili
senza dipendenze esterne (`g++ *.cpp -o crm`).

Requisiti chiave dal brief:
- OOP in C++ (classi, ereditarietà, polimorfismo)
- Struttura dati per clienti e interazioni
- Interfaccia a riga di comando interattiva, menu numerato a sottomenu
- CRUD completo sui clienti + ricerca parziale per nome/cognome
- Gestione interazioni (appuntamenti e contratti) collegate ai clienti
- Persistenza su file CSV, caricati all'avvio e salvati all'uscita

## Vincoli tecnici

- **Nessuna libreria esterna**: solo libreria standard C++ (niente
  FTXUI/ncurses). Scelta esplicita per evitare problemi di build in
  fase di consegna/valutazione.
- **Nessun sistema di build**: niente CMake. Compilazione manuale con
  `g++ *.cpp -o crm`, documentata nel README.
- **Multipiattaforma**: il codice non deve usare API specifiche di
  piattaforma (niente `<conio.h>`, niente codici ANSI obbligatori).
- Menu **numerato classico**: l'utente digita il numero della scelta e
  preme invio (niente navigazione con frecce, per evitare codice
  platform-specific).

## Struttura del progetto

```
InsuraProCRM/
├── main.cpp
├── Cliente.h / Cliente.cpp
├── Rubrica.h / Rubrica.cpp
├── Interazione.h / Interazione.cpp
├── Appuntamento.h / Appuntamento.cpp
├── Contratto.h / Contratto.cpp
├── StoricoInterazioni.h / StoricoInterazioni.cpp
├── FileManager.h / FileManager.cpp
├── Validatore.h / Validatore.cpp
├── Menu.h / Menu.cpp
├── clienti.csv
├── appuntamenti.csv
├── contratti.csv
└── README.md
```

## Classi e responsabilità

### Cliente
Rappresenta un singolo cliente. Campi:
- `id` (int, generato automaticamente)
- `nome`, `cognome` (string)
- `telefono` (string)
- `email` (string)
- `codiceFiscale` (string, 16 caratteri)
- `dataNascita` (string, formato gg/mm/aaaa)

Getter/setter con validazione base sui singoli campi (delegata a
`Validatore` per i controlli di formato).

**Comportamento sui valori non validi:** se un setter riceve un valore
che non supera la validazione di `Validatore`, **solleva
`std::invalid_argument`** con un messaggio che identifica chiaramente
sia il campo sia il motivo del rifiuto (es.
`"Email non valida: 'mario.rossi'"`,
`"Codice fiscale non valido: deve essere alfanumerico di 16 caratteri"`).
Il valore del campo resta invariato (nessuna scrittura parziale): il
chiamante che intercetta l'eccezione trova l'oggetto nello stato
precedente al tentativo. Questa è una scelta esplicita rispetto
all'alternativa "setter ritorna bool": si preferisce l'eccezione
perché il messaggio d'errore diventa parte del contratto dell'API e
può essere mostrato direttamente all'utente dal `Menu` tramite un
`catch (const std::invalid_argument& e)`, senza dover ricostruire il
messaggio a mano nel punto di chiamata.

### Rubrica
Gestisce la collezione di tutti i clienti (`std::vector<Cliente>`).
Responsabilità (CRUD + ricerca):
- `aggiungiCliente(...)`
- `visualizzaTutti()`
- `modificaCliente(id, ...)`
- `eliminaCliente(id)` — rimuove solo il cliente dalla rubrica; le
  interazioni collegate **non vengono toccate** (vedi sezione
  Eliminazione cliente)
- `cercaPerNomeCognome(query)` — match parziale, case-insensitive, su
  nome e/o cognome
- `getById(id)`
- gestione del contatore di ID clienti (prossimo ID = max ID letto dal
  file all'avvio + 1)

### Interazione (classe base astratta)
Campi comuni a tutte le interazioni:
- `id` (int, contatore condiviso tra appuntamenti e contratti)
- `idCliente` (int)
- `codiceFiscaleCliente` (string) — duplicato per riconciliazione
  anche se il cliente viene eliminato o un ID viene teoricamente
  riassegnato
- `data` (string, gg/mm/aaaa)

Metodi virtuali puri:
- `virtual std::string toCSV() const = 0;`
- `virtual void stampa() const = 0;`
- `virtual std::string getTipo() const = 0;` (per distinguere in
  visualizzazione/ricerca senza RTTI)
- Distruttore virtuale

### Appuntamento : public Interazione
Campi aggiuntivi:
- `orario` (string, hh:mm)
- `esito` (string, es. "concluso", "da ricontattare", "annullato")
- `note` (string, libera)

Implementa `toCSV()`, `stampa()`, `getTipo()` (ritorna "Appuntamento").

### Contratto : public Interazione
Campi aggiuntivi:
- `tipoPolizza` (string, es. "Auto", "Casa", "Vita")
- `premio` (double, positivo)
- `dataScadenza` (string, gg/mm/aaaa)

Implementa `toCSV()`, `stampa()`, `getTipo()` (ritorna "Contratto").

### StoricoInterazioni
Collezione **globale e unica** di tutte le interazioni:
`std::vector<std::unique_ptr<Interazione>>`.

Non esistono liste duplicate per singolo cliente: le interazioni di un
cliente sono ottenute filtrando questa collezione per `idCliente` (e
opzionalmente incrociando `codiceFiscaleCliente`).

Responsabilità:
- `aggiungiAppuntamento(...)` / `aggiungiContratto(...)`
- `getPerCliente(idCliente, codiceFiscale)` — ritorna le interazioni
  di un cliente specifico
- `cerca(criteri)` — ricerca su tutte le interazioni (es. per data, per
  tipo)
- `eliminaInterazione(id)` (se richiesto dall'utente esplicitamente)
- gestione del contatore condiviso di ID interazioni (prossimo ID =
  max ID letto tra `appuntamenti.csv` e `contratti.csv` + 1)

### FileManager
Unica classe che legge/scrive i file CSV. Nessun'altra classe accede
direttamente al filesystem.
- `caricaClienti(Rubrica&)`
- `caricaInterazioni(StoricoInterazioni&)`
- `salvaClienti(const Rubrica&)`
- `salvaInterazioni(const StoricoInterazioni&)`

Gestisce il parsing CSV con supporto per campi tra virgolette (per
note contenenti virgole) e la gestione delle righe malformate (vedi
Gestione errori).

### Validatore
Funzioni statiche riutilizzabili:
- `bool emailValida(string)` — presenza di "@" e un dominio con punto
- `bool codiceFiscaleValido(string)` — 16 caratteri alfanumerici
  (nessun controllo sull'algoritmo ufficiale del codice fiscale)
- `bool dataValida(string)` — formato gg/mm/aaaa, verificata come data
  reale (giorni per mese, anni bisestili)
- `bool numeroPositivo(double)`
- `bool campoNonVuoto(string)` — una stringa composta solo da
  spazi/whitespace (es. `"   "`) è considerata **vuota**: la funzione
  fa trim prima del controllo di lunghezza e ritorna `false` in quel
  caso. Evita che un utente inserisca "spazi" come escamotage per
  bypassare un campo obbligatorio.

**Politica di validazione applicata ovunque:** tutti i punti che
delegano a `Validatore` (setter di `Cliente`, costruttori/setter di
`Appuntamento` e `Contratto` per campi come `premio`, `data`,
`dataScadenza`) seguono la stessa regola dei setter di `Cliente`:
valore non valido → `std::invalid_argument` con messaggio specifico sul
campo, nessuna scrittura parziale dello stato. Questo rende uniforme
la gestione degli errori in tutto il livello dati, e permette al
`Menu` (unico punto che parla con l'utente) di avere un solo pattern
`try { ... } catch (const std::invalid_argument& e) { mostra e.what();
richiedi di reinserire solo quel campo }` riusabile identico ovunque.

### Menu
Gestisce solo l'interazione utente: stampa i (sotto)menu, legge input,
richiama i metodi giusti su `Rubrica` / `StoricoInterazioni`. Nessuna
logica di business o di file.

Struttura del menu:
```
MENU PRINCIPALE
1. Gestione Clienti
   1.1 Aggiungi cliente
   1.2 Visualizza tutti i clienti
   1.3 Modifica cliente
   1.4 Elimina cliente
   1.5 Cerca cliente
2. Gestione Interazioni
   2.1 Aggiungi appuntamento
   2.2 Aggiungi contratto
   2.3 Visualizza interazioni di un cliente
   2.4 Cerca interazioni
3. Esci (salva ed esce)
```

## Formato dati CSV

**clienti.csv**
```
id,nome,cognome,telefono,email,codice_fiscale,data_nascita
1,Mario,Rossi,3331234567,mario.rossi@email.com,RSSMRA80A01H501U,01/01/1980
```

**appuntamenti.csv**
```
id,id_cliente,codice_fiscale,data,orario,esito,note
1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,"Interessato a polizza casa"
```

**contratti.csv**
```
id,id_cliente,codice_fiscale,data,tipo_polizza,premio,data_scadenza
1,1,RSSMRA80A01H501U,15/03/2026,Auto,450.00,15/03/2027
```

Le note testuali sono racchiuse tra virgolette per gestire eventuali
virgole (standard CSV).

## Flusso applicativo

**Avvio (`main.cpp`):**
1. `FileManager` legge `clienti.csv` → popola `Rubrica`
2. `FileManager` legge `appuntamenti.csv` e `contratti.csv` → popola
   `StoricoInterazioni` (creando `Appuntamento`/`Contratto` concreti
   come `unique_ptr<Interazione>`)
3. Calcolo contatori: prossimo ID cliente = max ID clienti + 1;
   prossimo ID interazione = max ID tra i due file interazioni + 1
   (contatore condiviso)
4. Avvio del menu principale

**Durante l'esecuzione:** tutte le operazioni CRUD agiscono sulle
strutture dati in memoria (`Rubrica`, `StoricoInterazioni`).

**Uscita (voce "Esci"):** `FileManager` scrive lo stato corrente di
`Rubrica` e `StoricoInterazioni` nei 3 file CSV (sovrascrittura
completa), poi il programma termina.

## Eliminazione cliente

Quando un cliente viene eliminato dalla `Rubrica`:
- Le interazioni collegate **non vengono eliminate** dallo
  `StoricoInterazioni` né dai file CSV.
- Restano identificabili tramite `idCliente` + `codiceFiscaleCliente`
  salvati su ciascuna interazione, anche se in futuro un nuovo cliente
  dovesse ricevere lo stesso ID (scenario reso comunque improbabile
  dal contatore incrementale puro, ma coperto per robustezza).
- L'eliminazione esplicita di una singola interazione (se offerta
  dal menu) è un'azione separata e indipendente dall'eliminazione del
  cliente.

## Gestione errori

- **File CSV mancante all'avvio**: non è un errore; si parte con
  collezioni vuote, i file vengono creati al primo salvataggio.
- **Riga CSV malformata**: viene saltata con un messaggio di avviso a
  schermo (es. "Attenzione: riga N di contratti.csv ignorata, formato
  non valido"); il programma continua a caricare le righe successive.
- **Input menu non valido** (non numerico, fuori range): messaggio
  d'errore e ripresentazione dello stesso (sotto)menu, nessuna uscita
  dal programma.
- **Input che fallisce la validazione** (`Validatore`): il setter o il
  costruttore coinvolto solleva `std::invalid_argument` con un
  messaggio che identifica campo e motivo (vedi sezione Cliente e
  Validatore). Il `Menu` intercetta l'eccezione con
  `catch (const std::invalid_argument& e)`, mostra `e.what()` come
  messaggio d'errore e richiede di reinserire solo il campo non
  valido, senza perdere i campi già inseriti correttamente in quella
  stessa operazione.
- **Operazione su ID inesistente** (modifica/elimina/ricerca
  interazioni per un cliente non trovato): messaggio chiaro (es.
  "Cliente non trovato") e ritorno al menu, nessun crash.
- **Scrittura file fallita**: messaggio d'errore a schermo; il
  programma resta attivo con i dati in memoria intatti, l'utente può
  riprovare a salvare/uscire.

## Strategia di test

Nessun framework esterno (coerente con la scelta "niente dipendenze
esterne"):
- **Validatore**: funzioni pure, testabili con un piccolo set di casi
  noti (es. date valide/non valide, email valide/non valide) tramite
  un semplice programma/funzione di test dedicata.
- **Rubrica e StoricoInterazioni**: testabili in isolamento su dati in
  memoria (aggiungi/cerca/elimina), senza toccare file reali.
- **FileManager**: verifica manuale con CSV di prova contenenti casi
  limite (riga vuota, riga malformata, note con virgole).
- **Menu**: verifica manuale end-to-end (uso reale del programma), data
  la natura interattiva basata su `cin`.

## Fuori scope (YAGNI)

- Navigazione a frecce / interfacce grafiche o TUI avanzate
- Librerie esterne di qualsiasi tipo
- Sistemi di build (CMake, Makefile)
- Validazione formale del codice fiscale tramite l'algoritmo ufficiale
- Indirizzo cliente e altri campi anagrafici non richiesti
- Log su file degli errori (solo output a schermo)
- Eliminazione a cascata delle interazioni alla cancellazione di un
  cliente
