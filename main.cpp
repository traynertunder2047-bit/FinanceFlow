#include <iostream>
#include <cstdlib>
#include "./src/core/Bank.h"

///includo solo Bank.h perche' in questo file ho include Account.h perciò non c'e' bisogno di includerlo

/*Se Account.h e Account.cpp si trovano nella stessa identica cartella, ti basta scrivere semplicemente #include "Account.h", invece in main.cpp entri nella cartella perciò 
non c'è bisogno di specificare nuovamente il path*/

//per comppilare e eseguire: g++ main.cpp ./src/core/Account.cpp ./src/core/Bank.cpp -o trayner_bank ; ./trayner_bank

using namespace std;

int main() {
    Bank bank;

    cout << "<----------- Welcome to Trayner's Bank ------------->\n";

    // Fase iniziale: Creazione del primo account demo
    string username;
    string userPin;
    double userBalance;
    bool easyPin;

    cout << "Create your account:\n";
    
    // NUOVO CONTROLLO: Limite 35 caratteri per il primo account
    do {
        cout << "Enter a unique Username (no spaces, max 35 characters): ";
        cin >> username;
        if (username.size() > 35) {
            cout << "Username too long! Maximum length allowed is 35 characters. Try again.\n";
        }
    } while (username.size() > 35);

    do {
        cout << "Enter a strong PIN (exactly 8 characters with letters, numbers and symbols): ";
        cin >> userPin;
        if (!isValidPin(userPin)) {
            cout << "PIN invalid! Must be 8 characters long and contain letters, digits, and special characters.\n";
            easyPin = true;
        } else {
            cout << "Set PIN successfully!\n";
            easyPin = false;
        }
    } while (easyPin == true);

    cout << "Enter initial balance: ";
    cin >> userBalance;

    bank.addAccount(Account(username, userPin, userBalance));
    cout << "Account created successfully for user: " << username << ".\n";

    // Procedura di Login obbligatoria iniziale
    string loginUser;
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

        cout << "Enter Username: ";
        cin >> loginUser;
        cout << "Enter PIN: ";
        cin >> loginPin;

        userAccount = bank.findAccount(loginUser);
        if (userAccount == nullptr || userAccount->pin != loginPin) {
            loginAttempts++;
            cout << "Invalid Username or PIN. Attempt " << loginAttempts << "/" << MAX_LOGIN_ATTEMPTS << "\n";
            userAccount = nullptr;
        } else {
            cout << "Login successful!\n";
        }
    } while (userAccount == nullptr);

    // Ciclo Principale dell'Applicazione Bancaria
    int choice;
    string subInput;
    do {

        cout << "\n=====================================\n";
        cout << "<<<<<<<<<<<<< MAIN MENU >>>>>>>>>>>>>\n";
        cout << "=====================================\n";
        cout << "1) Financial Transactions\n";
        cout << "2) Account Management\n";
        cout << "3) Exit Program\n";
        cout << "-------------------------------------\n";
        cout << "Choose an option: ";
        cin >> subInput;

        if (subInput >= "1" && subInput <= "3" && subInput.size() == 1) {
            choice = stoi(subInput);
        } else {
            cout << "[ERROR]: Invalid option! Use numbers between 1-3.";
            choice = 0;
        }

        switch (choice) {
        case 1: { // --- SOTTO-MENU GESTIONE FINANZIARIA ---
            int subChoice;
            string subInput;
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
                cin >> subInput;
                
                if (subInput>= "1" && subInput <= "6" && subInput.size() == 1) {
                    subChoice = stoi(subInput); // conversione string -> int 
                } else {
                    cout << "[ERROR]: Invalid option! Use numbers between 1-6.";
                    subChoice = 0;
                }

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
                    bank.showAvailableDestinations(userAccount->username);

                    string destUser;
                    double amount;
                    cout << "\nEnter the destination Username (type 'cancel' to abort): ";
                    cin >> destUser;

                    if (destUser == "cancel") {
                        cout << "Transfer canceled.\n";
                        break;
                    }

                    Account *destAcc = bank.findAccount(destUser);
                    if (destAcc == nullptr || destAcc->username == userAccount->username) {
                        cout << "Error: Destination account invalid or inexistent.\n";
                        break;
                    }

                    cout << "Enter the amount to transfer: $";
                    cin >> amount;

                    if (bank.transfer(userAccount->username, destUser, amount)) {
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
            string subInput;
            do {
                cout << "\n--- Manage Account ---\n";
                cout << "1) Change account (Login)\n";
                cout << "2) Create additional bank account\n";
                cout << "3) Show total number of accounts\n";
                cout << "4) Back to Main Menu <---\n";
                cout << "----------------------\n";
                cout << "Choose an option: ";
                cin >> subInput;

                if (subInput >= "1" && subInput <= "4" && subInput.size() == 1) {
                    subChoice = stoi(subInput);
                } else {
                    cout << "[ERROR]: Invalid option! Use numbers between 1-4.";
                    subChoice = 0;
                }

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
                            if (userAccount->username == previousUser->username) {
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
                    string newUsername;
                    string newPin;
                    double newBalance;
                    
                    // NUOVO CONTROLLO: Limite 35 caratteri per l'account secondario
                    do {
                        cout << "Enter unique Username for the new account (no spaces, max 35 chars): ";
                        cin >> newUsername;
                        if (newUsername.size() > 35) {
                            cout << "Username too long! Maximum 35 characters allowed.\n";
                        }
                    } while (newUsername.size() > 35);
                    
                    do {
                        cout << "Set strong PIN (exactly 8 mixed characters): ";
                        cin >> newPin;
                    } while (!isValidPin(newPin));
                    
                    cout << "Enter initial deposit: ";
                    cin >> newBalance;

                    bank.addAccount(Account(newUsername, newPin, newBalance));
                    cout << "New account created for: " << newUsername << "\n";
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