#pragma once

#include <string>
#include <vector>

class Account {
public:
    int accountNumber;
    std::string pin;
    double balance;
    std::vector<std::string> transactionHistory;

    // Costruttore
    Account(int accNum, const std::string &pinStr, double bal);

    // Metodi
    void deposit(double amount);
    bool withdraw(double amount);
    void showHistory() const;
};

// Funzione globale ausiliaria per la validazione
bool isValidPin(const std::string &s);