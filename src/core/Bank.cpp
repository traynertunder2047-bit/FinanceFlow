#include "Bank.h"
#include <iostream>

void Bank::addAccount(const Account &acc) {
    accounts.push_back(std::make_unique<Account>(acc));
}

Account* Bank::findAccount(const std::string &user) {
    for (auto &acc : accounts) {
        if (acc->username == user) {
            return acc.get();
        }
    }
    return nullptr;
}

void Bank::showAvailableDestinations(const std::string &currentUsername) {
    int count = 0;
    for (auto &acc : accounts) {
        if (acc->username != currentUsername) {
            std::cout << "- Username: " << acc->username << "\n";
            count++;
        }
    }
    if (count == 0) {
        std::cout << "[No other accounts available for transfer]\n";
    }
}

bool Bank::transfer(const std::string &fromUser, const std::string &toUser, double amount) {
    Account* fromAcc = findAccount(fromUser);
    Account* toAcc = findAccount(toUser);
    
    if (!fromAcc || !toAcc) return false;
    if (amount <= 0 || fromAcc->balance < amount) return false;

    fromAcc->withdraw(amount);
    toAcc->deposit(amount);
    return true;
}

int Bank::getAccountCount() const {
    return static_cast<int>(accounts.size());
}

Account* login(Bank &bank) {
    std::string user;
    std::string pin;

    std::cout << "\n--- Login ---\n";
    std::cout << "Enter Username: ";
    std::cin >> user;
    
    // Controllo preventivo sulla lunghezza in fase di login
    if (user.size() > 35) {
        std::cout << "Invalid credentials (Username exceeds 35 characters).\n";
        return nullptr;
    }

    std::cout << "Enter PIN: ";
    std::cin >> pin;

    Account* acc = bank.findAccount(user);
    if (acc && acc->pin == pin) {
        std::cout << "Login successful!\n";
        return acc;
    }
    std::cout << "Invalid credentials.\n";
    return nullptr;
}