#include <iostream>
#include <cctype>//per funzioni che controllano i caratteri
#include "Account.h"

Account::Account(const std::string &user, const std::string &pinStr, double bal)
    : username(user), pin(pinStr), balance(bal) {
    transactionHistory.push_back("Account created for " + user + " with initial balance: $" + std::to_string(bal));
}

void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        std::cout << "Successfully deposited $" << amount << ".\n";
        transactionHistory.push_back("Deposited: +$" + std::to_string(amount));
    } else {
        std::cout << "Invalid deposit amount.\n";
    }
}

bool Account::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        std::cout << "Successfully withdrew $" << amount << ".\n";
        transactionHistory.push_back("Withdrew: -$" + std::to_string(amount));
        return true;
    } else {
        std::cout << "Error: insufficient balance or invalid amount.\n";
        return false;
    }
}

void Account::showHistory() const {
    std::cout << "\n--- Transaction History for " << username << " ---\n";
    if (transactionHistory.empty()) {
        std::cout << "No transactions yet.\n";
        return;
        //return; vuoto: Se il vettore è vuoto, stampiamo il messaggio e usciamo immediatamente dalla funzione usando return; senza passare dal ciclo.
    }
    for (const auto &entry : transactionHistory) {/*Sintassi for (const auto &entry : ...): Questo è un Range-based for loop (introdotto in C++11).
        Dice: "Per ogni elemento dentro transactionHistory, chiamalo entry e stampalo". auto fa capire automaticamente al compilatore che entry è una stringa.
        Il const & evita di sprecare memoria copiando la stringa a ogni giro del ciclo.*/ 
        std::cout << "- " << entry << "\n";
    }
}

// Controllo requisiti complessi del PIN (8 caratteri)
bool isValidPin(const std::string &s) {
    if (s.size() != 8) return false;

    bool hasLetter = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char c : s) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            hasLetter = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
        } else if (std::ispunct(static_cast<unsigned char>(c))) {
            hasSpecial = true;
        }
    }
    /*static_cast<unsigned_char>(C) è utilizzaro per evitare che il programma vada in crash perchè i char generalmente sono "negativi" e si usa questo per 
    convertirli in un formato sicuro e accettabile perchè le funzioni come isdigit si aspettano caratteri positivis*/
    return hasLetter && hasDigit && hasSpecial;
}