/* -> si utilizza per accedere a variabili e/o funzioni dell'oggetto puntato, si usa in pointers o in smart pointers
acc->accountNumber → accedi al dato
fromAcc->withdraw() → chiamo il metodo
*/
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <memory>

using namespace std; // meglio non includerlo in altri file pk funge da scatolone dove finiscono tutte le librerie e i nomi di funzioni, variabili, classi ecc.

class Account
{
public:
    int accountNumber;
    std::string pin;
    double balance;
    std::vector<std::string> transactionHistory; // <-- Nuova variabile per la cronologia

    Account(int accNum, const std::string &pinStr, double bal)
        : accountNumber(accNum), pin(pinStr), balance(bal)
    {
        // Registra la creazione del conto come prima transazione
        transactionHistory.push_back("Account created with initial balance: $" + std::to_string(bal));
    }

    void deposit(double amount)
    {
        if (amount > 0)
        {
            balance += amount;
            cout << "Successfully deposited $" << amount << ".\n";
            // Registra l'evento
            transactionHistory.push_back("Deposited: +$" + std::to_string(amount));
        }
        else
        {
            cout << "Invalid deposit amount.\n";
        }
    }

    bool withdraw(double amount)
    {
        if (amount > 0 && balance >= amount)
        {
            balance -= amount;
            cout << "Successfully withdrew $" << amount << ".\n";
            // Registra l'evento
            transactionHistory.push_back("Withdrew: -$" + std::to_string(amount));
            return true;
        }
        else
        {
            cout << "Error: insufficient balance or invalid amount.\n";
            return false;
        }
    }

    // Nuova funzione per mostrare la cronologia
    void showHistory() const
    {
        cout << "\n--- Transaction History for Account " << accountNumber << " ---\n";
        if (transactionHistory.empty())
        {
            cout << "No transactions yet.\n";
            return;
        }
        for (const auto &entry : transactionHistory)
        {
            cout << "- " << entry << "\n";
        }
    }
};

class Bank
{
private:
    // smart pointer, utilizzo std:: sempre per la questione di using namespace std; come contenitore
    std::vector<std::unique_ptr<Account>> accounts; /*vettore di puntatori unici a oggetti Account,
     garantisce che ogni account sia gestito in modo sicuro e che la memoria venga liberata automaticamente quando non è più necessaria*/

public:
    void addAccount(const Account &acc)
    {
        accounts.push_back(std::make_unique<Account>(acc));
    }

    Account *findAccount(int accNum)
    {
        for (auto &acc : accounts)
        {
            if (acc->accountNumber == accNum)
            { // vai all'oggetto Account puntato da acc e leggi accNum(numero account)
                return acc.get();
            }
        }
        return nullptr;
    }

    // per vedere gli account disponibili per il trasferimento soldi
    void showAvailableDestinations(int currentAccNum)
    {
        int count = 0;

        // da sottolineare cosa fa
        for (auto &acc : accounts)
        {
            if (acc->accountNumber != currentAccNum)
            { // Mostra solo i conti che NON sono quello attualmente in uso
                cout << "- Account Number: " << acc->accountNumber << "\n";
                count++;
            }
        }

        if (count == 0)
        {
            cout << "[No other accounts available for transfer]\n";
        }
    }

    // per trasferire i soldi
    // all return esce dalla funzione
    bool transfer(int fromAccNum, int toAccNum, double amount)
    {
        Account *fromAcc = findAccount(fromAccNum);
        Account *toAcc = findAccount(toAccNum);
        if (!fromAcc || !toAcc)
            return false;
        if (amount <= 0 || fromAcc->balance < amount)
            return false; // verifica se l'utente possiede ancora abbastanza soldi || se dall'account il bilancio è minore della quantita da trasferire

        // in entrambi i casi chiamo delle funzioni puntate dall puntatore in Account
        fromAcc->withdraw(amount);
        toAcc->deposit(amount);
        return true;
    }

    int getAccountCount() const
    {
        return static_cast<int>(accounts.size());
    }
};

// da controllare
bool isValidPin(const std::string &s)
{
    if (s.size() != 4)
        return false;
    for (char c : s)
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

// LOGIN
Account *login(Bank &bank)
{
    int accNum;
    std::string pin;

    cout << "\n--- Login ---\n";
    cout << "Enter account number: ";
    cin >> accNum;
    cout << "Enter PIN: ";
    cin >> pin;

    Account *acc = bank.findAccount(accNum);
    if (acc && acc->pin == pin)
    {
        cout << "Login successful!\n";
        return acc;
    }
    cout << "Invalid credentials.\n";
    return nullptr;
}

// NUMERO ACCOUNT
int generateAccountNumber()
{
    static int seed = 1000;
    seed++;
    return seed;
}

int main()
{
    Bank bank;
    srand(time(NULL));

    cout << "<----------- Welcome to Trayner's Bank ------------->\n";

    // Create an initial account for demo purposes
    int newAccNum = generateAccountNumber();
    std::string userPin;
    double userBalance;
    bool easyPin;

    cout << "Create your account:\n";

    do
    {
        cout << "Enter a PIN for your account (exactly 4 digits): ";
        cin >> userPin;
        if (!isValidPin(userPin))
        {
            cout << "PIN must be exactly 4 numeric digits (no spaces or special characters).\n";
            easyPin = true;
        }
        else if (userPin == "1234" || userPin == "4321" || userPin == "0000" || userPin == "1111")
        {
            cout << "PIN too common, please try again!\n";
            easyPin = true;
        }
        else
        {
            cout << "Set PIN successfully!\n";
            easyPin = false;
        }
    } while (easyPin == true);

    cout << "Enter initial balance: ";
    cin >> userBalance;

    bank.addAccount(Account(newAccNum, userPin, userBalance));
    cout << "Account created! Your account number is " << newAccNum << ".\n";

    // Login process
    int loginAccNum;
    std::string loginPin;
    Account *userAccount = nullptr;
    int loginAttempts = 0;
    const int MAX_LOGIN_ATTEMPTS = 3;

    cout << "\nPlease log in.\n";
    do
    {
        if (loginAttempts >= MAX_LOGIN_ATTEMPTS)
        {
            cout << "Too many failed login attempts. Exiting.\n";
            return 0;
        }

        cout << "Enter account number: ";
        cin >> loginAccNum;
        cout << "Enter PIN: ";
        cin >> loginPin;

        // for checking if the account pin corresponds to the login pin
        userAccount = bank.findAccount(loginAccNum);
        if (userAccount == nullptr || userAccount->pin != loginPin)
        {
            loginAttempts++;
            cout << "Invalid account number or PIN. Attempt " << loginAttempts << "/" << MAX_LOGIN_ATTEMPTS << "\n";
            userAccount = nullptr;
        }
        else
        {
            cout << "Login successful!\n";
        }
    } while (userAccount == nullptr);

    // Nuova struttura del ciclo di transazioni con sotto-menu persistenti
    int choice;
    do
    {
        cout << "\n=====================================\n";
        cout << "<<<<<<<<<<<<< MAIN MENU >>>>>>>>>>>>>\n";
        cout << "=====================================\n";
        cout << "1) Financial Transactions\n";
        cout << "2) Account Management\n";
        cout << "3) Exit Program\n";
        cout << "-------------------------------------\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        { // --- SOTTO-MENU GESTIONE FINANZIARIA ---
            int subChoice;
            do
            {
                cout << "\n--- Financial Transactions ---\n";
                cout << "1) Check balance\n";
                cout << "2) Deposit money\n";
                cout << "3) Withdraw money\n";
                cout << "4) Transfer money\n";
                cout << "5) View Transaction History\n";
                cout << "6) Back to Main Menu <---\n"; // Nuova opzione di uscita dal sotto-menu
                cout << "------------------------------\n";
                cout << "Choose an option: ";
                cin >> subChoice;

                switch (subChoice)
                {
                case 1:
                    cout << "\nYour current balance: $" << userAccount->balance << "\n";//accede al bilancio in userAccount
                    break;
                case 2:
                {
                    double amount;
                    cout << "Enter amount to deposit: $";
                    cin >> amount;
                    userAccount->deposit(amount);//amount come parametro della funzione deposit al quale userAccount accedeattraverso il puntatore
                    break;
                }
                case 3:
                {
                    double amount;
                    cout << "Enter amount to withdraw: $";
                    cin >> amount;
                    userAccount->withdraw(amount);
                    break;
                }
                case 4:
                {
                    cout << "\nAvailable accounts for transfer:\n";
                    bank.showAvailableDestinations(userAccount->accountNumber);

                    int destAccNum;
                    double amount;
                    cout << "\nEnter the destination account number (0 to cancel): ";
                    cin >> destAccNum;

                    if (destAccNum == 0)
                    {
                        cout << "Transfer canceled.\n";
                        break;
                    }

                    Account *destAcc = bank.findAccount(destAccNum);
                    if (destAcc == nullptr || destAcc->accountNumber == userAccount->accountNumber)
                    {
                        cout << "Error: Destination account invalid or inexistent.\n";
                        break;
                    }

                    cout << "Enter the amount to transfer: $";
                    cin >> amount;

                    if (bank.transfer(userAccount->accountNumber, destAccNum, amount))
                    {
                        cout << "Transfer completed successfully!\n";
                    }
                    else
                    {
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
            } while (subChoice != 6); // Rimane qui dentro finché non digita 6
            break;
        }

        case 2:
        { // --- SOTTO-MENU GESTIONE ACCOUNT ---
            int subChoice;
            do
            {
                cout << "\n--- Manage Account ---\n";
                cout << "1) Change account (Login)\n";
                cout << "2) Create additional bank account\n";
                cout << "3) Show total number of accounts\n";
                cout << "4) Back to Main Menu <---\n"; // Nuova opzione di uscita dal sotto-menu
                cout << "----------------------\n";
                cout << "Choose an option: ";
                cin >> subChoice;

                switch (subChoice)
                {
                case 1:
                { // Change account (login)
                    cout << "\n--- Change Account ---\n";
                    Account *previousUser = userAccount;
                    int changeAttempts = 0;
                    userAccount = nullptr;
                    bool selfLoginError = false;

                    while (!userAccount && changeAttempts < MAX_LOGIN_ATTEMPTS)
                    {
                        userAccount = login(bank);
                        if (userAccount != nullptr)
                        {
                            if (userAccount->accountNumber == previousUser->accountNumber)
                            {
                                cout << "\n[ERRORE]: Sei gia' autenticato con questo account.\n";
                                selfLoginError = true;
                                break;
                            }
                        }
                        else
                        {
                            changeAttempts++;
                            cout << "Login attempt " << changeAttempts << "/" << MAX_LOGIN_ATTEMPTS << " failed.\n";
                        }
                    }

                    if (!userAccount || selfLoginError)
                    {
                        if (!selfLoginError)
                        {
                            cout << "Too many failed login attempts. Returning to menu.\n";
                        }
                        userAccount = previousUser; // Ripristina il vecchio account in caso di errore
                    }
                    break;
                }
                case 2:
                { // Create account
                    int newAccNum = generateAccountNumber();
                    std::string newPin;
                    double newBalance;
                    do
                    {
                        cout << "Set PIN (exactly 4 digits): ";
                        cin >> newPin;
                    } while (!isValidPin(newPin) || newPin == "1234" || newPin == "4321" || newPin == "1111" || newPin == "0000");
                    cout << "Enter initial deposit: ";
                    cin >> newBalance;

                    bank.addAccount(Account(newAccNum, newPin, newBalance));
                    cout << "New account created: " << newAccNum << "\n";
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
            } while (subChoice != 4); // Rimane qui dentro finché non digita 4
            break;
        }

        case 3:
            cout << "\nThank you for using Trayner's Bank. Goodbye!\n";
            break;

        default:
            cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 3); // Il programma si chiude solo se l'utente digita 3 dal Main Menu

    return 0;
}