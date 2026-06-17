#pragma once

#include <string>
#include <vector>

class Account {
public:
    std::string username; // Sostituito accountNumber con username
    std::string pin;
    double balance;
    std::vector<std::string> transactionHistory;

    // Costruttore aggiornato
    Account(const std::string &user, const std::string &pinStr, double bal);

    // Metodi
    void deposit(double amount);
    bool withdraw(double amount);
    void showHistory() const;//il const per indicare al compialtore che questo metodo è in sola lettura ovvero mostrerà i dati in ouput ma non modificherà nessuna variabile interna all'account
};

// Nuova funzione di validazione del PIN forte (8 caratteri, misto)
bool isValidPin(const std::string &s);