#pragma once

#include "Account.h"
#include <vector>
#include <memory>//per utilizzare gli smart pointers

class Bank {
private:
    std::vector<std::unique_ptr<Account>> accounts;

public:
    void addAccount(const Account &acc);
    Account* findAccount(const std::string &user); // Ricerca per username
    void showAvailableDestinations(const std::string &currentUsername);
    bool transfer(const std::string &fromUser, const std::string &toUser, double amount);
    int getAccountCount() const;
};

// Funzione globale di interfaccia bancaria
Account* login(Bank &bank);