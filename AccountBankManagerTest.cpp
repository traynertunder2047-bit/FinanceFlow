
#include <iostream>
#include <vector>
using namespace std;

//reppresents a single bank account
class Account {
public:
    int accountNumber;//the accounts idefication number
    int pin; 
    double balance;

    //Constructor, this list is called in inizialization list, its used to inizialize the members of the class with past values 
    Account(int accNum, int pinNum, double bal)
        : accountNumber(accNum), pin(pinNum), balance(bal) {}

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Succesfully deposited $" << amount << "\n";
        } else {
            cout << "Invalid deposit amount.\n";
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            cout << "Succesfully withdraw $" << amount << "\n";
            return true;
        }
        cout << "Error: insufficient balance or invalid amount.\n";
        return false;
    }
};

class Bank {
private:
    vector<Account> accounts;

public:
    void addAccount(const Account& acc) {
        accounts.push_back(acc);
    }
    //Find the account number
    Account* findAccount(int accNum) {
        for (auto& acc : accounts) {
            if (acc.accountNumber == accNum)
                return &acc;
        }
        return nullptr;
    }
    //To transfer the amount of money 
    bool transfer(int from, int to, double amount) {
        Account* fromAcc = findAccount(from);
        Account* toAcc = findAccount(to);

        if (!fromAcc || !toAcc) return false;
        if (fromAcc->withdraw(amount)) {
            toAcc->deposit(amount);
            return true;
        }
        return false;
    }
};

//generates the account's number
int generateAccountNumber() {
    static int acc = 1000;
    return ++acc;
}

//pin validation
bool validPin(int pin) {
    if (pin < 0 || pin > 9999) return false;
    if (pin == 1234 || pin == 4321 || pin == 1111 || pin == 0000)
        return false;
    return true;
}

/* Creates a new account */
Account createAccount() {
    int pin;
    double balance;

    do {
        cout << "Inserisci un PIN (4 cifre): ";
        cin >> pin;
    } while (!validPin(pin));

    cout << "Saldo iniziale: ";
    cin >> balance;

    return Account(generateAccountNumber(), pin, balance);
}

//login 
Account* login(Bank& bank) {
    int accNum, pin;
    cout << "Account number: ";
    cin >> accNum;
    cout << "PIN: ";
    cin >> pin;

    Account* acc = bank.findAccount(accNum);
    if (acc && acc->pin == pin) {
        cout << "Login was succesfull.\n";
        return acc;
    }

    cout << "Error: invalid credentials.\n";
    return nullptr;
}


//menu and everything else
int main() {
    Bank bank;

    cout << "<----- Welcome to trayner's Bank ----->\n";

    // First account 
    Account first = createAccount();
    bank.addAccount(first);
    cout << "Account Created! Account number: " << first.accountNumber << "\n";

    Account* user = nullptr;
    while (!user) user = login(bank);

    int choice;
    do {
        cout << "\n--- MENU ---\n";
        cout << "1) Balance\n";
        cout << "2) Deposit\n";
        cout << "3) Withdraw\n";
        cout << "4) Transfer\n";
        cout << "5) New account\n";
        cout << "6) Change account\n";
        cout << "7) Exit\n";
        cout << "Choice: ";
        cin >> choice;

        
        switch (choice) {
            //check balance
            case 1: {
                cout << "Balance: " << user->balance << " $\n";
                break;
            }
            //deposit a certain amount of money
            case 2: {
                double amount;
                cout << "Importo: ";
                cin >> amount;
                user->deposit(amount);
                break;
            }
            //Withdraw a certain amount of money
            case 3: {
                double amount;
                cout << "Importo: ";
                cin >> amount;
                user->withdraw(amount);
                break;
            }
            //Transfer a certain amount of money to an other account
            case 4: {
                int dest;
                double amount;
                cout << "Conto destinatario: ";
                cin >> dest;
                cout << "Importo: ";
                cin >> amount;

                if (bank.transfer(user->accountNumber, dest, amount))
                    cout << "Trasfer compleated.\n";
                else
                    cout << "Error in the transfer.\n";//temporaneo
                break;
            }
            //Create a new account
            case 5: {
                Account newAcc = createAccount();
                bank.addAccount(newAcc);
                cout << "New account created: " << newAcc.accountNumber << "\n";
                break;
            }
            //Login in the new account
            case 6:
                user = nullptr;
                while (!user) user = login(bank);
                break;
            //Exit the program
            case 7:
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Scelta non valida.\n";
        }

    } while (choice != 7);

    return 0;
}

