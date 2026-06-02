#pragma once

#include "Account.h"
#include <vector>
#include <memory>

class Bank {
private:
    std::vector<std::unique_ptr<Account>> accounts;

public:
    // Metodi della classe Bank
    void addAccount(const Account &acc);
    Account* findAccount(int accNum);
    void showAvailableDestinations(int currentAccNum);
    bool transfer(int fromAccNum, int toAccNum, double amount);
    int getAccountCount() const;
};

// Funzioni globali di interfaccia bancaria
Account* login(Bank &bank);
int generateAccountNumber();