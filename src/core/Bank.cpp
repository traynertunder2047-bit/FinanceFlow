#include "Bank.h"
#include <iostream>

void Bank::addAccount(const Account &acc) {
    accounts.push_back(std::make_unique<Account>(acc));
}

Account* Bank::findAccount(int accNum) {
    for (auto &acc : accounts) {
        if (acc->accountNumber == accNum) {
            return acc.get();
        }
    }
    return nullptr;
}

void Bank::showAvailableDestinations(int currentAccNum) {
    int count = 0;
    for (auto &acc : accounts) {
        if (acc->accountNumber != currentAccNum) {
            std::cout << "- Account Number: " << acc->accountNumber << "\n";
            count++;
        }
    }
    if (count == 0) {
        std::cout << "[No other accounts available for transfer]\n";
    }
}

bool Bank::transfer(int fromAccNum, int toAccNum, double amount) {
    Account* fromAcc = findAccount(fromAccNum);
    Account* toAcc = findAccount(toAccNum);
    
    if (!fromAcc || !toAcc) return false;
    if (amount <= 0 || fromAcc->balance < amount) return false;

    // Esegue prelievo e deposito chiamando i metodi dell'oggetto
    fromAcc->withdraw(amount);
    toAcc->deposit(amount);
    return true;
}

int Bank::getAccountCount() const {
    return static_cast<int>(accounts.size());
}

Account* login(Bank &bank) {
    int accNum;
    std::string pin;

    std::cout << "\n--- Login ---\n";
    std::cout << "Enter account number: ";
    std::cin >> accNum;
    std::cout << "Enter PIN: ";
    std::cin >> pin;

    Account* acc = bank.findAccount(accNum);
    if (acc && acc->pin == pin) {
        std::cout << "Login successful!\n";
        return acc;
    }
    std::cout << "Invalid credentials.\n";
    return nullptr;
}

int generateAccountNumber() {
    static int seed = 1000;
    seed++;
    return seed;
}