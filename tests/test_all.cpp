// =============================================================================
// test_all.cpp - test manuali per tutte le funzionalita' del CRM
// =============================================================================
// Non usa nessun framework esterno: solo la libreria standard C++ (come da
// scelte di progetto). L'idea e' semplicissima:
//   - ogni controllo passa da verifica(condizione, "descrizione");
//   - alla fine si stampa un riepilogo con il numero di test passati/falliti;
//   - il programma esce con codice 0 se e' tutto verde, 1 se qualcosa fallisce
//     (comodo per script e integrazione continua).
//
// COMPILAZIONE (dalla cartella del progetto, NON includere main.cpp o si hanno
// due funzioni main). Su una sola riga:
//
//   g++ -std=c++14 tests/test_all.cpp src/*.cpp -o tests/test_all
//
// ESECUZIONE:
//   ./tests/test_all         (Windows: tests\test_all.exe)
// =============================================================================

#include "../src/Validatore.h"
#include "../src/Cliente.h"
#include "../src/Rubrica.h"
#include "../src/Interazione.h"
#include "../src/Appuntamento.h"
#include "../src/Contratto.h"
#include "../src/StoricoInterazioni.h"
#include "../src/FileManager.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>   // std::function (per l'aiutante "lancia")
#include <fstream>      // per creare file di prova nei test del FileManager
#include <cstdio>       // std::remove (cancella i file di prova)

// -----------------------------------------------------------------------------
// Contatori globali dei test e funzione di verifica
// -----------------------------------------------------------------------------
static int totaleTest = 0;
static int testFalliti = 0;

// Registra un controllo: stampa [OK] o [FAIL] e aggiorna i contatori.
void verifica(bool condizione, const std::string& descrizione) {
    totaleTest++;
    if (condizione) {
        std::cout << "  [OK]   " << descrizione << "\n";
    } else {
        testFalliti++;
        std::cout << "  [FAIL] " << descrizione << "\n";
    }
}

// Esegue "azione" e dice se ha sollevato un std::invalid_argument.
// Serve per testare i casi in cui ci aspettiamo che il codice rifiuti un
// valore non valido lanciando un'eccezione.
bool lancia(const std::function<void()>& azione) {
    try {
        azione();
        return false;                       // non ha lanciato nulla
    } catch (const std::invalid_argument&) {
        return true;                        // ha lanciato l'eccezione attesa
    } catch (...) {
        return false;                       // ha lanciato un'eccezione diversa
    }
}

// =============================================================================
// 1) VALIDATORE
// =============================================================================
void testValidatore() {
    std::cout << "\n== Validatore ==\n";

    // --- trim ---
    verifica(Validatore::trim("  ciao  ") == "ciao", "trim: toglie spazi ai bordi");
    verifica(Validatore::trim("\t\n x \r") == "x",   "trim: toglie anche tab e a-capo");
    verifica(Validatore::trim("     ").empty(),       "trim: stringa di soli spazi diventa vuota");

    // --- campoNonVuoto ---
    verifica(!Validatore::campoNonVuoto(""),        "campoNonVuoto: \"\" e' vuoto");
    verifica(!Validatore::campoNonVuoto("    "),    "campoNonVuoto: soli spazi = vuoto");
    verifica(Validatore::campoNonVuoto("  x  "),    "campoNonVuoto: c'e' un carattere vero");

    // --- emailValida ---
    verifica(Validatore::emailValida("mario.rossi@email.com"), "email valida: mario.rossi@email.com");
    verifica(Validatore::emailValida("a@b.co"),                "email valida: forma minima a@b.co");
    verifica(!Validatore::emailValida("senzachiocciola"),      "email non valida: manca la @");
    verifica(!Validatore::emailValida("a@@b.com"),             "email non valida: due @");
    verifica(!Validatore::emailValida("@b.com"),               "email non valida: parte locale vuota");
    verifica(!Validatore::emailValida("a@bcom"),               "email non valida: dominio senza punto");
    verifica(!Validatore::emailValida("a@.com"),               "email non valida: punto all'inizio del dominio");
    verifica(!Validatore::emailValida("a@b."),                 "email non valida: punto alla fine del dominio");
    // Scelta di progetto: la validazione e' volutamente permissiva.
    verifica(Validatore::emailValida("a@b..c"),                "email: i punti doppi sono ammessi (validazione semplice)");

    // --- codiceFiscaleValido ---
    verifica(Validatore::codiceFiscaleValido("RSSMRA80A01H501U"),   "CF valido: 16 caratteri alfanumerici");
    verifica(Validatore::codiceFiscaleValido("0000000000000000"),   "CF valido: 16 cifre");
    verifica(Validatore::codiceFiscaleValido("  RSSMRA80A01H501U "), "CF valido: gli spazi ai bordi vengono ignorati");
    verifica(!Validatore::codiceFiscaleValido("RSSMRA80A01H501"),    "CF non valido: 15 caratteri");
    verifica(!Validatore::codiceFiscaleValido("RSSMRA80A01H501UU"),  "CF non valido: 17 caratteri");
    verifica(!Validatore::codiceFiscaleValido("RSSMRA80A01H501!"),   "CF non valido: contiene un simbolo");

    // --- dataValida ---
    verifica(Validatore::dataValida("01/01/1980"),  "data valida: 01/01/1980");
    verifica(Validatore::dataValida("29/02/2000"),  "data valida: 29/02/2000 (bisestile, div. per 400)");
    verifica(Validatore::dataValida("29/02/2004"),  "data valida: 29/02/2004 (bisestile)");
    verifica(Validatore::dataValida("31/03/2020"),  "data valida: 31/03/2020 (marzo ha 31 giorni)");
    verifica(Validatore::dataValida("31/12/2999"),  "data valida: 31/12/2999 (limite superiore anno)");
    verifica(!Validatore::dataValida("29/02/2001"), "data non valida: 29/02/2001 (non bisestile)");
    verifica(!Validatore::dataValida("31/04/2020"), "data non valida: 31/04/2020 (aprile ha 30 giorni)");
    verifica(!Validatore::dataValida("00/01/2020"), "data non valida: giorno 00");
    verifica(!Validatore::dataValida("01/00/2020"), "data non valida: mese 00");
    verifica(!Validatore::dataValida("01/13/2020"), "data non valida: mese 13");
    verifica(!Validatore::dataValida("01/01/1899"), "data non valida: anno prima del 1900");
    verifica(!Validatore::dataValida("1/1/2020"),   "data non valida: formato senza zeri (lunghezza sbagliata)");
    verifica(!Validatore::dataValida(""),           "data non valida: stringa vuota");
    verifica(!Validatore::dataValida("aa/bb/cccc"), "data non valida: non sono cifre");

    // --- numeroPositivo ---
    verifica(Validatore::numeroPositivo(0.0),   "numeroPositivo: 0 e' accettato");
    verifica(Validatore::numeroPositivo(12.5),  "numeroPositivo: 12.5");
    verifica(!Validatore::numeroPositivo(-0.1), "numeroPositivo: -0.1 rifiutato");
}

// =============================================================================
// 2) CLIENTE
// =============================================================================
void testCliente() {
    std::cout << "\n== Cliente ==\n";

    // Costruzione con dati validi: i getter devono restituire quei valori.
    Cliente c(1, "Mario", "Rossi", "3331234567",
              "mario.rossi@email.com", "RSSMRA80A01H501U", "01/01/1980");
    verifica(c.getId() == 1,                          "Cliente: getId");
    verifica(c.getNome() == "Mario",                  "Cliente: getNome");
    verifica(c.getCognome() == "Rossi",              "Cliente: getCognome");
    verifica(c.getTelefono() == "3331234567",        "Cliente: getTelefono");
    verifica(c.getEmail() == "mario.rossi@email.com","Cliente: getEmail");
    verifica(c.getCodiceFiscale() == "RSSMRA80A01H501U", "Cliente: getCodiceFiscale");
    verifica(c.getDataNascita() == "01/01/1980",     "Cliente: getDataNascita");

    // Costruzione con dati non validi: il costruttore deve lanciare.
    verifica(lancia([] {
        Cliente x(2, "", "Rossi", "333", "m@e.com", "RSSMRA80A01H501U", "01/01/1980");
    }), "Cliente: nome vuoto -> eccezione");
    verifica(lancia([] {
        Cliente x(2, "Mario", "Rossi", "333", "nonvalida", "RSSMRA80A01H501U", "01/01/1980");
    }), "Cliente: email non valida -> eccezione");
    verifica(lancia([] {
        Cliente x(2, "Mario", "Rossi", "333", "m@e.com", "CF-CORTO", "01/01/1980");
    }), "Cliente: codice fiscale non valido -> eccezione");
    verifica(lancia([] {
        Cliente x(2, "Mario", "Rossi", "333", "m@e.com", "RSSMRA80A01H501U", "31/02/1980");
    }), "Cliente: data di nascita non valida -> eccezione");

    // Setter valido: cambia il valore.
    c.setNome("Luigi");
    verifica(c.getNome() == "Luigi", "Cliente: setNome valido aggiorna il campo");

    // Setter non valido: lancia e NON modifica il campo (nessuna scrittura parziale).
    bool haLanciato = lancia([&] { c.setEmail("rotta"); });
    verifica(haLanciato && c.getEmail() == "mario.rossi@email.com",
             "Cliente: setEmail non valido lancia e lascia il valore precedente");
}

// =============================================================================
// 3) RUBRICA
// =============================================================================
void testRubrica() {
    std::cout << "\n== Rubrica ==\n";

    Rubrica r;
    int id1 = r.aggiungiCliente("Mario", "Rossi", "333", "m@e.com",
                                "RSSMRA80A01H501U", "01/01/1980");
    int id2 = r.aggiungiCliente("Anna", "Verdi", "334", "a@e.com",
                                "VRDNNA85B02H501X", "02/02/1985");
    verifica(id1 == 1 && id2 == 2,               "Rubrica: aggiungiCliente assegna id progressivi");
    verifica(r.visualizzaTutti().size() == 2,    "Rubrica: visualizzaTutti riflette gli inserimenti");

    // getById
    Cliente trovato(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    verifica(r.getById(id1, trovato) && trovato.getNome() == "Mario",
             "Rubrica: getById trova un cliente esistente");
    Cliente nonToccato(0, "segnaposto", "y", "y", "y@y.com", "1111111111111111", "02/02/2002");
    verifica(!r.getById(999, nonToccato) && nonToccato.getNome() == "segnaposto",
             "Rubrica: getById su id inesistente ritorna false e non tocca il parametro");

    // modificaCliente
    verifica(r.modificaCliente(id1, "Mario", "Bianchi", "333", "m@e.com",
                               "RSSMRA80A01H501U", "01/01/1980"),
             "Rubrica: modificaCliente su id esistente ritorna true");
    r.getById(id1, trovato);
    verifica(trovato.getCognome() == "Bianchi", "Rubrica: modificaCliente aggiorna i dati");
    verifica(!r.modificaCliente(999, "A", "B", "C", "a@b.com",
                                "RSSMRA80A01H501U", "01/01/1980"),
             "Rubrica: modificaCliente su id inesistente ritorna false");

    // modifica con un dato non valido: lancia e il cliente resta invariato
    bool modificaLancia = lancia([&] {
        r.modificaCliente(id1, "Mario", "Bianchi", "333", "EMAIL-ROTTA",
                          "RSSMRA80A01H501U", "01/01/1980");
    });
    r.getById(id1, trovato);
    verifica(modificaLancia && trovato.getEmail() == "m@e.com",
             "Rubrica: modificaCliente con dato non valido lancia e non modifica");

    // cercaPerNomeCognome (parziale, case-insensitive, su nome e cognome)
    std::vector<Cliente> perNome = r.cercaPerNomeCognome("ann");
    verifica(perNome.size() == 1 && perNome[0].getNome() == "Anna",
             "Rubrica: cerca per nome parziale");
    std::vector<Cliente> perCognome = r.cercaPerNomeCognome("BIA");
    verifica(perCognome.size() == 1 && perCognome[0].getCognome() == "Bianchi",
             "Rubrica: cerca e' case-insensitive e guarda anche il cognome");
    verifica(r.cercaPerNomeCognome("zzz").empty(),
             "Rubrica: cerca senza corrispondenze ritorna vuoto");

    // cercaPerNomeECognome (campi separati, verifica solo su quelli non vuoti)
    // Stato attuale: id1 = Mario Bianchi, id2 = Anna Verdi.
    std::vector<Cliente> soloNome = r.cercaPerNomeECognome("mario", "");
    verifica(soloNome.size() == 1 && soloNome[0].getCognome() == "Bianchi",
             "Rubrica: cercaPerNomeECognome con solo il nome filtra solo su quello");
    std::vector<Cliente> soloCognome = r.cercaPerNomeECognome("", "verdi");
    verifica(soloCognome.size() == 1 && soloCognome[0].getNome() == "Anna",
             "Rubrica: cercaPerNomeECognome con solo il cognome filtra solo su quello");
    verifica(r.cercaPerNomeECognome("mario", "verdi").empty(),
             "Rubrica: cercaPerNomeECognome richiede che ENTRAMBI i campi combacino quando sono presenti");
    verifica(r.cercaPerNomeECognome("", "").size() == 2,
             "Rubrica: cercaPerNomeECognome con entrambi i campi vuoti ritorna tutti i clienti");
    std::vector<Cliente> entrambi = r.cercaPerNomeECognome("MARIO", "BIA");
    verifica(entrambi.size() == 1 && entrambi[0].getNome() == "Mario",
             "Rubrica: cercaPerNomeECognome e' case-insensitive e parziale su entrambi i campi");

    // eliminaCliente
    verifica(r.eliminaCliente(id2) && r.visualizzaTutti().size() == 1,
             "Rubrica: eliminaCliente rimuove il cliente");
    verifica(!r.eliminaCliente(id2),
             "Rubrica: eliminaCliente su id gia' rimosso ritorna false");

    // contatore id
    r.impostaProssimoId(50);
    verifica(r.getProssimoId() == 50, "Rubrica: impostaProssimoId / getProssimoId");
    int id3 = r.aggiungiCliente("Tre", "Terzi", "3", "t@e.com",
                                "0000000000000000", "03/03/2003");
    verifica(id3 == 50, "Rubrica: aggiungiCliente usa il contatore impostato");
}

// =============================================================================
// 4) INTERAZIONE / APPUNTAMENTO / CONTRATTO (ereditarieta' e polimorfismo)
// =============================================================================
void testInterazioni() {
    std::cout << "\n== Appuntamento / Contratto ==\n";

    // --- Appuntamento ---
    Appuntamento app(1, 10, "RSSMRA80A01H501U", "15/03/2026", "10:30",
                     "concluso", "nota con, virgola");
    verifica(app.getId() == 1 && app.getIdCliente() == 10, "Appuntamento: id e idCliente");
    verifica(app.getData() == "15/03/2026",               "Appuntamento: data (dalla classe base)");
    verifica(app.getTipo() == "Appuntamento",             "Appuntamento: getTipo");
    verifica(app.getOrario() == "10:30" && app.getEsito() == "concluso",
             "Appuntamento: campi propri (orario, esito)");
    verifica(app.toCSV() ==
             "1,10,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"nota con, virgola\"",
             "Appuntamento: toCSV mette tra virgolette le note con virgola");

    verifica(!lancia([] { Appuntamento a(1, 1, "CF", "01/01/2020", "09:00", "ok", ""); }),
             "Appuntamento: le note vuote sono ammesse");
    verifica(lancia([] { Appuntamento a(1, 1, "CF", "DATA-ROTTA", "09:00", "ok", "x"); }),
             "Appuntamento: data non valida -> eccezione");
    verifica(lancia([] { Appuntamento a(1, 1, "CF", "01/01/2020", "", "ok", "x"); }),
             "Appuntamento: orario vuoto -> eccezione");
    verifica(lancia([] { Appuntamento a(1, 1, "CF", "01/01/2020", "09:00", "", "x"); }),
             "Appuntamento: esito vuoto -> eccezione");

    // --- Contratto ---
    Contratto con(2, 10, "RSSMRA80A01H501U", "15/03/2026", "Auto", 450.0, "15/03/2027");
    verifica(con.getTipo() == "Contratto",                 "Contratto: getTipo");
    verifica(con.getTipoPolizza() == "Auto" && con.getPremio() == 450.0,
             "Contratto: campi propri (tipo polizza, premio)");
    verifica(con.getDataScadenza() == "15/03/2027",        "Contratto: data scadenza");
    verifica(con.toCSV() ==
             "2,10,RSSMRA80A01H501U,15/03/2026,Auto,450.000000,15/03/2027",
             "Contratto: toCSV");

    verifica(!lancia([] { Contratto c(1, 1, "CF", "01/01/2020", "Auto", 0.0, "01/01/2021"); }),
             "Contratto: premio 0 e' ammesso");
    verifica(lancia([] { Contratto c(1, 1, "CF", "01/01/2020", "Auto", -1.0, "01/01/2021"); }),
             "Contratto: premio negativo -> eccezione");
    verifica(lancia([] { Contratto c(1, 1, "CF", "01/01/2020", "", 10.0, "01/01/2021"); }),
             "Contratto: tipo polizza vuoto -> eccezione");
    verifica(lancia([] { Contratto c(1, 1, "CF", "01/01/2020", "Auto", 10.0, "ROTTA"); }),
             "Contratto: data scadenza non valida -> eccezione");

    // --- Polimorfismo: un puntatore alla classe base chiama il metodo giusto ---
    Appuntamento appPoli(5, 5, "CF", "01/01/2020", "09:00", "ok", "");
    Contratto conPoli(6, 6, "CF", "01/01/2020", "Vita", 99.0, "01/01/2021");
    Interazione* base1 = &appPoli;
    Interazione* base2 = &conPoli;
    verifica(base1->getTipo() == "Appuntamento" && base2->getTipo() == "Contratto",
             "Polimorfismo: Interazione* chiama la getTipo della classe concreta");
}

// =============================================================================
// 5) STORICO INTERAZIONI
// =============================================================================
void testStorico() {
    std::cout << "\n== StoricoInterazioni ==\n";

    StoricoInterazioni s;
    int a1 = s.aggiungiAppuntamento(1, "CF1", "01/01/2026", "09:00", "ok", "");
    int c1 = s.aggiungiContratto(1, "CF1", "02/01/2026", "Auto", 100.0, "02/01/2027");
    int a2 = s.aggiungiAppuntamento(2, "CF2", "03/01/2026", "10:00", "ok", "");
    verifica(a1 == 1 && c1 == 2 && a2 == 3,
             "Storico: il contatore id e' condiviso fra appuntamenti e contratti");
    verifica(s.tutte().size() == 3, "Storico: tutte() riflette gli inserimenti");

    // getPerCliente
    verifica(s.getPerCliente(1, "").size() == 2,
             "Storico: getPerCliente filtra per idCliente");
    verifica(s.getPerCliente(1, "CF1").size() == 2,
             "Storico: getPerCliente con codice fiscale corretto trova le interazioni");
    verifica(s.getPerCliente(1, "ALTRO").empty(),
             "Storico: getPerCliente con codice fiscale diverso non trova nulla");
    verifica(s.getPerCliente(999, "").empty(),
             "Storico: getPerCliente per un cliente senza interazioni ritorna vuoto");

    // eliminaInterazione
    verifica(s.eliminaInterazione(c1) && s.tutte().size() == 2,
             "Storico: eliminaInterazione rimuove per id");
    verifica(!s.eliminaInterazione(c1),
             "Storico: eliminaInterazione su id gia' rimosso ritorna false");

    // contatore
    s.impostaProssimoId(10);
    verifica(s.getProssimoId() == 10, "Storico: impostaProssimoId / getProssimoId");
}

// =============================================================================
// 6) FILE MANAGER - parsing CSV
// =============================================================================
void testParseCSV() {
    std::cout << "\n== FileManager::parseRigaCSV ==\n";

    std::vector<std::string> c1 = FileManager::parseRigaCSV("a,b,c");
    verifica(c1.size() == 3 && c1[0] == "a" && c1[2] == "c",
             "parseRigaCSV: riga semplice a tre campi");

    std::vector<std::string> c2 = FileManager::parseRigaCSV("1,\"a,b\",c");
    verifica(c2.size() == 3 && c2[1] == "a,b",
             "parseRigaCSV: la virgola dentro le virgolette non separa");

    std::vector<std::string> c3 = FileManager::parseRigaCSV("x,\"ha detto \"\"ciao\"\"\",y");
    verifica(c3.size() == 3 && c3[1] == "ha detto \"ciao\"",
             "parseRigaCSV: le virgolette raddoppiate diventano una virgoletta");

    std::vector<std::string> c4 = FileManager::parseRigaCSV("");
    verifica(c4.empty(), "parseRigaCSV: riga vuota -> nessun campo");

    std::vector<std::string> c5 = FileManager::parseRigaCSV("a,,c");
    verifica(c5.size() == 3 && c5[1].empty(), "parseRigaCSV: campo centrale vuoto");

    std::vector<std::string> c6 = FileManager::parseRigaCSV("a,b,");
    verifica(c6.size() == 3 && c6[2].empty(), "parseRigaCSV: campo finale vuoto");
}

// =============================================================================
// 7) FILE MANAGER - salvataggio e ricaricamento (round-trip su file veri)
// =============================================================================
void testFileManagerRoundTrip() {
    std::cout << "\n== FileManager: salva e ricarica ==\n";

    const std::string fClienti = "test_tmp_clienti.csv";
    const std::string fApp     = "test_tmp_appuntamenti.csv";
    const std::string fContr   = "test_tmp_contratti.csv";

    // --- Clienti: salvo due clienti, li ricarico in una Rubrica nuova ---
    Rubrica rOut;
    rOut.aggiungiCliente("Mario", "Rossi", "333", "m@e.com",
                         "RSSMRA80A01H501U", "01/01/1980");
    rOut.aggiungiCliente("Anna", "Verdi", "334", "a@e.com",
                         "VRDNNA85B02H501X", "02/02/1985");
    FileManager::salvaClienti(rOut, fClienti);

    Rubrica rIn;
    FileManager::caricaClienti(rIn, fClienti);
    verifica(rIn.visualizzaTutti().size() == 2,
             "FileManager: dopo il round-trip la Rubrica ha 2 clienti");
    Cliente tmp(0, "x", "x", "x", "x@x.com", "0000000000000000", "01/01/2000");
    verifica(rIn.getById(2, tmp) && tmp.getNome() == "Anna",
             "FileManager: id e dati dei clienti si conservano");
    verifica(rIn.getProssimoId() == 3,
             "FileManager: caricaClienti imposta il contatore a maxId + 1");

    // --- Interazioni: salvo un appuntamento (con nota che contiene una virgola)
    //     e un contratto, poi li ricarico ---
    StoricoInterazioni sOut;
    sOut.aggiungiAppuntamento(1, "RSSMRA80A01H501U", "15/03/2026", "10:30",
                              "concluso", "nota con, virgola");
    sOut.aggiungiContratto(2, "VRDNNA85B02H501X", "16/03/2026", "Casa",
                           300.0, "16/03/2027");
    FileManager::salvaInterazioni(sOut, fApp, fContr);

    StoricoInterazioni sIn;
    FileManager::caricaInterazioni(sIn, fApp, fContr);
    verifica(sIn.tutte().size() == 2,
             "FileManager: dopo il round-trip lo Storico ha 2 interazioni");
    verifica(sIn.getProssimoId() == 3,
             "FileManager: caricaInterazioni imposta il contatore condiviso a maxId + 1");

    std::vector<const Interazione*> perCliente1 = sIn.getPerCliente(1, "");
    verifica(perCliente1.size() == 1 && perCliente1[0]->getTipo() == "Appuntamento",
             "FileManager: l'interazione viene ricreata con il tipo corretto");
    // Il modo piu' semplice per controllare che la nota con virgola sia
    // sopravvissuta al giro su file: riconvertire in CSV e confrontare.
    verifica(perCliente1[0]->toCSV() ==
             "1,1,RSSMRA80A01H501U,15/03/2026,10:30,concluso,\"nota con, virgola\"",
             "FileManager: le note con virgola sopravvivono al salva/ricarica");

    // --- Riga malformata: viene saltata, le altre si caricano lo stesso ---
    const std::string fMal = "test_tmp_malformato.csv";
    std::ofstream out(fMal);
    out << "id,nome,cognome,telefono,email,codice_fiscale,data_nascita\n";
    out << "1,Mario,Rossi,333,m@e.com,RSSMRA80A01H501U,01/01/1980\n";
    out << "questa,riga,ha,pochi,campi\n";                                  // 5 campi
    out << "3,Anna,Verdi,334,EMAIL-ROTTA,VRDNNA85B02H501X,02/02/1985\n";     // email non valida
    out << "4,Luca,Bianchi,335,l@e.com,BNCLCU90C03H501Y,03/03/1990\n";
    out.close();

    std::cout << "  (nota: qui sotto compaiono 2 avvisi su righe ignorate: e' voluto)\n";
    Rubrica rMal;
    FileManager::caricaClienti(rMal, fMal);
    verifica(rMal.visualizzaTutti().size() == 2,
             "FileManager: le righe malformate vengono saltate, le buone caricate");
    verifica(rMal.getProssimoId() == 5,
             "FileManager: il contatore si basa sul max id delle sole righe valide");

    // --- Pulizia: cancello i file di prova ---
    std::remove(fClienti.c_str());
    std::remove(fApp.c_str());
    std::remove(fContr.c_str());
    std::remove(fMal.c_str());
}

// =============================================================================
// main: esegue tutti i gruppi di test e stampa il riepilogo
// =============================================================================
int main() {
    std::cout << "=== TEST CRM InsuraPro ===\n";

    testValidatore();
    testCliente();
    testRubrica();
    testInterazioni();
    testStorico();
    testParseCSV();
    testFileManagerRoundTrip();

    std::cout << "\n=== RIEPILOGO ===\n";
    std::cout << "Test eseguiti: " << totaleTest << "\n";
    std::cout << "Falliti:       " << testFalliti << "\n";

    if (testFalliti == 0) {
        std::cout << "TUTTO OK\n";
        return 0;
    }
    std::cout << "CI SONO TEST FALLITI\n";
    return 1;
}
