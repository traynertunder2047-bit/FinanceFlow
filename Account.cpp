#include "Account.h"
#include <iostream>
#include <cctype>

Account::Account(int accNum, const std::string &pinStr, double bal)
    : accountNumber(accNum), pin(pinStr), balance(bal) {
    transactionHistory.push_back("Account created with initial balance: $" + std::to_string(bal));
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
    std::cout << "\n--- Transaction History for Account " << accountNumber << " ---\n";
    if (transactionHistory.empty()) {
        std::cout << "No transactions yet.\n";
        return;
    }
    for (const auto &entry : transactionHistory) {
        std::cout << "- " << entry << "\n";
    }
}

bool isValidPin(const std::string &s) {
    if (s.size() != 4) return false;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}