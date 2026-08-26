#include "Validatore.h"
#include <string>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace Validatore {
    // Funzione per eliminare gli spazi all'inizio e alla fine di una stringa
    std::string trim(const std::string& s){
        size_t inizio = s.find_first_not_of(" \t\n\r");
        if (inizio == std::string::npos) {
            return "";
        }
        size_t fine = s.find_last_not_of(" \t\n\r");
        return s.substr(inizio, fine - inizio + 1);
    }

    // Funzione che elimina tutti gli spazi da una stringa
    std::string rimuoviSpazi(std::string s) {
        s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
        return s;
    }

    // Funzione che valida un indirizzo email
    bool emailValida(const std::string& email){

        int cont_chiocciola = 0;
        int pos_chiocciola = 0;

        // Eliminazione degli spazi all'inizio e alla fine della stringa
        std::string emailPulita = trim(email);        

        // L'indirizzo può essere lungo al massimo 254 caratteri
        if (emailPulita.length() > 254){
            return false;
        }

        // Ciclo for che conta il numero di @ presenti e individua la posizione dell'ultima @
        for (int i=0; i < emailPulita.length(); i++){
            if (emailPulita[i] == '@'){
                pos_chiocciola = i;
                cont_chiocciola++;
            }
        }
        // Se ci sono più @ la mail non è valida
        if (cont_chiocciola > 1){
            return false;
        }
        // Se la mail inizia con @ o se la @ non è presente la mail non è valida
        if (pos_chiocciola == 0){
            return false;
        }
        // Separo l'indirizzo email nelle sue componenti e definisco i caratteri speciali ammessi
        std::string locale = emailPulita.substr(0, pos_chiocciola);
        std::string dominio = emailPulita.substr(pos_chiocciola + 1, emailPulita.length() - pos_chiocciola);
        // Individuo l'ultimo '.' del dominio per individuare l'estensione
        int ultimo_punto = dominio.rfind('.');
        // Se non viene trovato nessun '.' la mail non è valida
        if (ultimo_punto == std::string::npos){
            return false;
        }
        std::string estensione = dominio.substr(ultimo_punto + 1, dominio.length() - ultimo_punto);
        std::string caratteri_ammessi_locale = "._%+-";
        std::string caratteri_ammessi_dominio = ".-";

        // Validazione della parte locale
        // La parte locale non può essere vuota
        if (locale.length() == 0){
            return false;
        } 
        // La parte locale non può iniziare o finnire con un '.'
        if ((locale[0] == '.') || (locale[locale.length() - 1] == '.')){
            return false;
        } 
        // La parte locale può contenere solo caratteri alfanumerici o . _ % + -
        // La parte locale non può contenere due '.' consecutivi
        for (int i=0; i < locale.length(); i++){
            if ((i != 0) && (locale[i] == '.') && (locale[i-1] == '.')){
                return false;
            }
            if (!(std::isalnum(locale[i])) && (caratteri_ammessi_locale.find(locale[i]) == std::string::npos)){
                return false;
            }
        }
        // Validazione del dominio
        // Il dominio può essere lungo al più 255 caratteri
        if (dominio.length() > 255){
            return false;
        }
        // Il dominio non può essere vuoto
        if (dominio.length() == 0){
            return false;
        } 
        // L'estensione deve avere almeno due caratteri
        if (estensione.length() < 2){
            return false;
        }       

        // Il dominio deve contenere almeno un punto per la separazione dall'estensione finale
        for (int i=0; i < dominio.length(); i++){
            // Il dominio non può iniziare o finire con un '.' o '-'
            if (((i == 0) || (i == dominio.length() - 1)) && ((dominio[i] == '.') || (dominio[i] == '-'))){
                return false;
            }
            // Il dominio può contenere solo alfanumerici, '.' o '-'
            if (!(std::isalnum(dominio[i])) && (caratteri_ammessi_dominio.find(dominio[i]) == std::string::npos)){
                return false;
            }
        }
        // L'estensione può contenere solo lettere dell'alfabeto
        for (int i=0; i < estensione.length(); i++){
            if (!(std::isalpha(estensione[i]))){
                return false;
            }
        }       
        // Se tutte le condizioni di esclusione sono false, la funzione restituisce true
        return true;
    }

    // Funzione per la validazione del codice fiscale 
    bool codiceFiscaleValido(const std::string& cf){

        std::string lettere_mese = "ABCDEHLMPRST";
        std::string cfPulito = trim(cf); // eliminazione degli spazi all'inizio e alla fine
        // Conversione in maiuscolo  
        for (int i = 0; i < cfPulito.length(); i++) {
            cfPulito[i] = toupper(cfPulito[i]);
        }

        // Il CF deve essere lungo 16 caratteri
        if (cfPulito.length() != 16) {
            return false;
        }
        // I caratteri da 0 a 5devono essere al alfabetici (nome e cognome)
        for (int i=0; i<=5; i++){
            if (!isalpha(cfPulito[i])){
                return false;
            }
        }
        // I caratteri 6 e 7 devono essere numerici (anno di nascita)
        for (int i=6; i<=7; i++){
            if (!isdigit(cfPulito[i])){
                return false;
            }
        }
        // il carattere 8 deve contenere una delle lettere associate ai mesi
        if ((lettere_mese.find(cfPulito[8]) == std::string::npos)){
            return false;
        }
        // I caratteri 9 e 10 (giorno di nascita) possono contenere valori da 01 a 31 o da 41 a 71
        if (!isdigit(cfPulito[9]) || !isdigit(cfPulito[10])) {
            return false;
        }
        int giorno = std::stoi(cfPulito.substr(9, 1));
        if (!((giorno >= 1 && giorno <= 31) || (giorno >= 41 && giorno <= 71))) {
            return false;
        }
        // il carattere 11 (iniziale codice catastale comune) deve essere una lettera
        if (!isalpha(cfPulito[11])){
            return false;
        } 
        // I caratteri da 12 a 14 devono essere numerici (codice catastale
        for (int i=12; i<=14; i++){
            if (!isdigit(cfPulito[i])){
                return false;
            }
        }
        // il carattere 15 deve contenere una lettera (carattere di controllo)
            if (!isalpha(cfPulito[15])){
            return false;
        } 

        return true;        
    }

    // Funzione che verifica se la data è nel formato valido "dd/mm/yyyy"
    bool dataValida(const std::string& data){
        std::string dataPulita = trim(data); // Eliminazione degli spazi all'inizio e alla fine della stinga
        std::string mesi31 = "_1_3_5_7_8_10_12";
        
        // i caratteri 2 e 5 devono contenere '/'
        if (!((dataPulita[2] == '/') && (dataPulita[5] == '/'))){
            return false;
        }
        // i caratteri relativi a dd, mm e yyyy devono essere delle cifre
        for (int i=0; i<=1; i++){
            if (!(isdigit(dataPulita[i]))){
                return false;
            }
        }
        for (int i=3; i<=4; i++){
            if (!(isdigit(dataPulita[i]))){
                return false;
            }
        }
        for (int i=6; i<=9; i++){
            if (!(isdigit(dataPulita[i]))){
                return false;
            }
        }
        
        // converto dd, mm e yyyy in interi
        std::string giorno_str = dataPulita.substr(0, 2);
        std::string mese_str = dataPulita.substr(3, 2);
        std::string anno_str = dataPulita.substr(6, 4);
        int giorno = std::stoi(giorno_str);
        int mese = std::stoi(mese_str);
        int anno = std::stoi(anno_str);

        std::cout << dataPulita << std::endl;
        std::cout << giorno_str + " " + mese_str + " " + anno_str << std::endl;

        if (mese < 1 || mese > 12){
            return false;
        } 

        bool bisestile = (anno % 4 == 0 && anno % 100 != 0) || (anno % 400 == 0);
        int giorniMassimi;

        if (mesi31.find(mese_str) != std::string::npos) {
            giorniMassimi = 31;
        } 
        else if (mese == 2) {
            if (bisestile){
                
                giorniMassimi = 29;
            }
            else{
                giorniMassimi = 28;
            }
        } 
        else {
            giorniMassimi = 30;
        }

        if (giorno < 1 || giorno > giorniMassimi){
            return false;
        }
        
        
        // anno compreso tra 1900 e 2999
        if ((anno < 1900) || (anno > 2999)){  
            return false;
        }

        return true;

    }
    
    bool numeroPositivo(double valore){
        if (valore < 0){
            return false;
        }
        else { 
            return true;
        }
    }

    bool campoNonVuoto(const std::string& valore) {
        return !trim(valore).empty();
    }

    
    bool numeroTelefonoValido(const std::string& numero){
        std::string numeroPulito = rimuoviSpazi(numero); // eliminazione degli spazi all'inizio e alla fine
        if (numeroPulito.length() < 7 || numeroPulito.length() > 15){
            return false;
        }
        if ((numeroPulito[0] != '+') && !(isdigit(numeroPulito[0]))){
            return false;
        }

        for (int i=1; i<numeroPulito.length(); i++){
            if (!isdigit(numeroPulito[i])){
                return false;
            }
        }

        return true;
    }
        

    bool tipoInterazioneValido(const std::string& tipo){
        std::string tipoPulito = trim(tipo); // eliminazione degli spazi all'inizio e alla fine
        for (int i = 0; i < tipoPulito.length(); i++) {
            tipoPulito[i] = toupper(tipoPulito[i]);
        }

        if ((tipoPulito == "CONTRATTO") || (tipoPulito == "APPUNTAMENTO")){
            return true;
        }
        else{
            return false;
        }
    }
}
