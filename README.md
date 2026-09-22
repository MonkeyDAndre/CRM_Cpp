# CRM InsuraPro Solutions

![CI](https://github.com/MonkeyDAndre/CRM_Cpp/actions/workflows/ci.yml/badge.svg)

CRM a riga di comando per la gestione di clienti e delle loro interazioni
(appuntamenti e contratti), sviluppato in C++ standard (C++14) senza
dipendenze esterne e senza sistema di build: basta `g++`.

## Funzionalità

- **Gestione clienti**: aggiunta, modifica, eliminazione, ricerca per nome
  e/o cognome (case-insensitive, parziale, con AND solo sui campi compilati).
- **Gestione interazioni**: appuntamenti e contratti, collegati a un cliente
  tramite id e codice fiscale; ricerca per data.
- **Persistenza su CSV**: i dati vengono caricati all'avvio e salvati
  automaticamente all'uscita, in tre file separati (`clienti.csv`,
  `appuntamenti.csv`, `contratti.csv`). Il caricamento è tollerante: righe
  malformate vengono scartate con un avviso, senza bloccare l'avvio.
- **Menu interattivo curato**: ogni schermata pulisce il terminale e mostra
  solo le informazioni rilevanti in quel momento, con un titolo dedicato;
  i risultati restano visibili finché non si preme INVIO per continuare.
  Durante l'inserimento di un valore, digitando `&` si annulla l'operazione
  e si torna al menu precedente.

## Compilazione

Dalla cartella del progetto:

```
g++ -std=c++14 -Wall -Wextra main.cpp src/*.cpp -o crm.exe
```

(su Linux/macOS: `g++ -std=c++14 -Wall -Wextra main.cpp src/*.cpp -o crm`)

Nessuna dipendenza esterna, nessun sistema di build richiesto.

## Esecuzione

```
./crm.exe
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
   5. Cerca cliente (nome e/o cognome)
2. Gestione Interazioni
   1. Aggiungi appuntamento
   2. Aggiungi contratto
   3. Visualizza interazioni di un cliente
   4. Cerca interazioni (per data)
3. Esci (salva ed esce)
```

## Struttura del progetto

```
main.cpp              punto di ingresso
src/                   classi del dominio (Cliente, Rubrica, Interazione,
                       Appuntamento, Contratto, StoricoInterazioni,
                       FileManager, Menu, Validatore)
tests/test_all.cpp    test suite (harness custom, nessun framework esterno)
DESIGN.md             design dettagliato di ogni classe, formati CSV,
                       policy di validazione
PLAN.md               piano di implementazione TDD originale
```

## Test

Il progetto include una test suite manuale (nessun framework esterno,
solo libreria standard) con 113 verifiche su validazione, CRUD, ricerca,
persistenza CSV e polimorfismo:

```
g++ -std=c++14 -Wall -Wextra tests/test_all.cpp src/*.cpp -o tests/test_all.exe
./tests/test_all.exe
```

Viene eseguita automaticamente ad ogni push tramite GitHub Actions (vedi
badge in cima a questo file).

## Note di design

- L'eliminazione di un cliente non elimina le sue interazioni: restano
  consultabili tramite l'ID cliente e il codice fiscale salvati su
  ciascuna interazione.
- I valori non validi (email, codice fiscale, date, importi) vengono
  rifiutati con un messaggio che spiega il motivo, chiedendo di
  reinserire solo il campo incriminato.
- Approfondimenti su design delle classi e formati CSV: [DESIGN.md](DESIGN.md).

## Licenza

Distribuito sotto licenza [MIT](LICENSE).
