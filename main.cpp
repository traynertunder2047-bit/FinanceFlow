#include <iostream>
#include <ctime>
#include <cstdlib>
#include "Bank.h"//includo solo Bank.h perche' in questo file ho include Account.h perciò non c'e' bisogno di includerlo

//per comppilare e eseguire: g++ main.cpp Account.cpp Bank.cpp -o trayner_bank ; ./trayner_bank

using namespace std;

int main() {
    Bank bank;
    srand(static_cast<unsigned int>(time(NULL)));

    cout << "<----------- Welcome to Trayner's Bank ------------->\n";

    // Fase iniziale: Creazione del primo account demo
    int newAccNum = generateAccountNumber();
    string userPin;
    double userBalance;
    bool easyPin;

    cout << "Create your account:\n";
    do {
        cout << "Enter a PIN for your account (exactly 4 digits): ";
        cin >> userPin;
        if (!isValidPin(userPin)) {
            cout << "PIN must be exactly 4 numeric digits (no spaces or special characters).\n";
            easyPin = true;
        } else if (userPin == "1234" || userPin == "4321" || userPin == "0000" || userPin == "1111") {
            cout << "PIN too common, please try again!\n";
            easyPin = true;
        } else {
            cout << "Set PIN successfully!\n";
            easyPin = false;
        }
    } while (easyPin == true);

    cout << "Enter initial balance: ";
    cin >> userBalance;

    bank.addAccount(Account(newAccNum, userPin, userBalance));
    cout << "Account created! Your account number is " << newAccNum << ".\n";

    // Procedura di Login obbligatoria iniziale
    int loginAccNum;
    string loginPin;
    Account *userAccount = nullptr;
    int loginAttempts = 0;
    const int MAX_LOGIN_ATTEMPTS = 3;

    cout << "\nPlease log in.\n";
    do {
        if (loginAttempts >= MAX_LOGIN_ATTEMPTS) {
            cout << "Too many failed login attempts. Exiting.\n";
            return 0;
        }

        cout << "Enter account number: ";
        cin >> loginAccNum;
        cout << "Enter PIN: ";
        cin >> loginPin;

        userAccount = bank.findAccount(loginAccNum);
        if (userAccount == nullptr || userAccount->pin != loginPin) {
            loginAttempts++;
            cout << "Invalid account number or PIN. Attempt " << loginAttempts << "/" << MAX_LOGIN_ATTEMPTS << "\n";
            userAccount = nullptr;
        } else {
            cout << "Login successful!\n";
        }
    } while (userAccount == nullptr);

    // Ciclo Principale dell'Applicazione Bancaria
    int choice;
    do {
        cout << "\n=====================================\n";
        cout << "<<<<<<<<<<<<< MAIN MENU >>>>>>>>>>>>>\n";
        cout << "=====================================\n";
        cout << "1) Financial Transactions\n";
        cout << "2) Account Management\n";
        cout << "3) Exit Program\n";
        cout << "-------------------------------------\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
        case 1: { // --- SOTTO-MENU GESTIONE FINANZIARIA ---
            int subChoice;
            do {
                cout << "\n--- Financial Transactions ---\n";
                cout << "1) Check balance\n";
                cout << "2) Deposit money\n";
                cout << "3) Withdraw money\n";
                cout << "4) Transfer money\n";
                cout << "5) View Transaction History\n";
                cout << "6) Back to Main Menu <---\n";
                cout << "------------------------------\n";
                cout << "Choose an option: ";
                cin >> subChoice;

                switch (subChoice) {
                case 1:
                    cout << "\nYour current balance: $" << userAccount->balance << "\n";
                    break;
                case 2: {
                    double amount;
                    cout << "Enter amount to deposit: $";
                    cin >> amount;
                    userAccount->deposit(amount);
                    break;
                }
                case 3: {
                    double amount;
                    cout << "Enter amount to withdraw: $";
                    cin >> amount;
                    userAccount->withdraw(amount);
                    break;
                }
                case 4: {
                    cout << "\nAvailable accounts for transfer:\n";
                    bank.showAvailableDestinations(userAccount->accountNumber);

                    int destAccNum;
                    double amount;
                    cout << "\nEnter the destination account number (0 to cancel): ";
                    cin >> destAccNum;

                    if (destAccNum == 0) {
                        cout << "Transfer canceled.\n";
                        break;
                    }

                    Account *destAcc = bank.findAccount(destAccNum);
                    if (destAcc == nullptr || destAcc->accountNumber == userAccount->accountNumber) {
                        cout << "Error: Destination account invalid or inexistent.\n";
                        break;
                    }

                    cout << "Enter the amount to transfer: $";
                    cin >> amount;

                    if (bank.transfer(userAccount->accountNumber, destAccNum, amount)) {
                        cout << "Transfer completed successfully!\n";
                    } else {
                        cout << "Transfer failed. Insufficient funds or invalid amount.\n";
                    }
                    break;
                }
                case 5:
                    userAccount->showHistory();
                    break;
                case 6:
                    cout << "\nReturning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid financial option. Try again.\n";
                }
            } while (subChoice != 6);
            break;
        }

        case 2: { // --- SOTTO-MENU GESTIONE ACCOUNT ---
            int subChoice;
            do {
                cout << "\n--- Manage Account ---\n";
                cout << "1) Change account (Login)\n";
                cout << "2) Create additional bank account\n";
                cout << "3) Show total number of accounts\n";
                cout << "4) Back to Main Menu <---\n";
                cout << "----------------------\n";
                cout << "Choose an option: ";
                cin >> subChoice;

                switch (subChoice) {
                case 1: { // Cambio account (Login)
                    cout << "\n--- Change Account ---\n";
                    Account *previousUser = userAccount;
                    int changeAttempts = 0;
                    userAccount = nullptr;
                    bool selfLoginError = false;

                    while (!userAccount && changeAttempts < MAX_LOGIN_ATTEMPTS) {
                        userAccount = login(bank);
                        if (userAccount != nullptr) {
                            if (userAccount->accountNumber == previousUser->accountNumber) {
                                cout << "\n[ERRORE]: Sei gia' autenticato con questo account.\n";
                                selfLoginError = true;
                                break;
                            }
                        } else {
                            changeAttempts++;
                            cout << "Login attempt " << changeAttempts << "/" << MAX_LOGIN_ATTEMPTS << " failed.\n";
                        }
                    }

                    if (!userAccount || selfLoginError) {
                        if (!selfLoginError) {
                            cout << "Too many failed login attempts. Returning to menu.\n";
                        }
                        userAccount = previousUser; // Ripristina la sessione precedente
                    }
                    break;
                }
                case 2: { // Creazione account aggiuntivo
                    int newAcc = generateAccountNumber();
                    string newPin;
                    double newBalance;
                    do {
                        cout << "Set PIN (exactly 4 digits): ";
                        cin >> newPin;
                    } while (!isValidPin(newPin) || newPin == "1234" || newPin == "4321" || newPin == "1111" || newPin == "0000");
                    cout << "Enter initial deposit: ";
                    cin >> newBalance;

                    bank.addAccount(Account(newAcc, newPin, newBalance));
                    cout << "New account created: " << newAcc << "\n";
                    break;
                }
                case 3:
                    cout << "\nCurrent number of accounts: " << bank.getAccountCount() << "\n";
                    break;
                case 4:
                    cout << "\nReturning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid account management option. Try again.\n";
                }
            } while (subChoice != 4);
            break;
        }

        case 3:
            cout << "\nThank you for using Trayner's Bank. Goodbye!\n";
            break;

        default:
            cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 3);

    return 0;
}