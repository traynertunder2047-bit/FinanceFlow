/*Cose da cambiare
1-- dividere le classi in file diversi per rendere il codice piu organizzato(verso la fine)
2-- prima di creare tutta la UI migliorare la grafica da terminale rimediando alla continua ripetizione del menu che porta a rendere il programma un po più confusionario,
3-- migliorare la UI delle risposte dei case tipo del bilancio, trasferimento, ritiro e altro, magari renderli un po più visibili/notabili e meno nascosti dal menu
4-- quando chiede di depositare il denaro questo viene depositato senza alcuna traccia della provenienza, rimediare magari con l'aggiunta di un modo per guadagnare 
denaro, per esempio con la compravendita di azioni e criptovalute
5-- ERRORE -> case 6 -> quando avviene il login in un account non avviene la verifica del caso dove l'utente puo inserire le stesse credenziali(PIN e ACCNumber), magari è meglio fare
come nel case 4 (Transfer) dove se viene messo lo stesso ACCNumber di quello che l'utente utilizza il programma si 'ferma' e dice all'utente che non è possibile compiere quell'azione e
ritorna al menu {{{RISOLTO}}}
*/


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

using namespace std;//meglio non includerlo in altri file pk funge da scatolone dove finiscono tutte le librerie e i nomi di funzioni, variabili, classi ecc.

class Account {
public:
    int accountNumber;
    std::string pin;
    double balance;

    /*inizializzo delle variabili attraverso una lista di inizializzazione, i 2 : indicano al compilatore di costruire e assegnare le variabili nel momento della creazione dell'oggetto*/
    Account(int accNum, const std::string &pinStr, double bal)
        : accountNumber(accNum), pin(pinStr), balance(bal) {}

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Successfully deposited $" << amount << ".\n";
        } else {
            cout << "Invalid deposit amount.\n";
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            cout << "Successfully withdrew $" << amount << ".\n";
            return true;
        } else {
            cout << "Error: insufficient balance or invalid amount.\n";
            return false;
        }
    }

};

class Bank {
private:
    //smart pointer, utilizzo std:: sempre per la questione di using namespace std; come contenitore 
    std::vector<std::unique_ptr<Account>> accounts;/*vettore di puntatori unici a oggetti Account, 
    garantisce che ogni account sia gestito in modo sicuro e che la memoria venga liberata automaticamente quando non è più necessaria*/
    

public:
    void addAccount(const Account &acc) {
        accounts.push_back(std::make_unique<Account>(acc));
    }

    Account* findAccount(int accNum) {
        for (auto &acc : accounts) {
            if (acc->accountNumber == accNum) {//vai all'oggetto Account puntato da acc e leggi accNum(numero account) 
                return acc.get();
            }
        }
        return nullptr;
    }

    //per vedere gli account disponibili per il trasferimento soldi
    void showAvailableDestinations(int currentAccNum) {
        int count = 0;

            //da sottolineare cosa fa
            for (auto &acc : accounts) {
                if (acc->accountNumber != currentAccNum) {// Mostra solo i conti che NON sono quello attualmente in uso
                cout << "- Account Number: " << acc->accountNumber << "\n";
                count++;
            }
        }

        if (count == 0) {
            cout << "[No other accounts available for transfer]\n";
        }
    }

    //per trasferire i soldi
    //all return esce dalla funzione 
    bool transfer(int fromAccNum, int toAccNum, double amount) {
        Account* fromAcc = findAccount(fromAccNum);
        Account* toAcc = findAccount(toAccNum);
        if (!fromAcc || !toAcc) return false;
        if (amount <= 0 || fromAcc->balance < amount) return false;//verifica se l'utente possiede ancora abbastanza soldi || se dall'account il bilancio è minore della quantita da trasferire

        //in entrambi i casi chiamo delle funzioni puntate dall puntatore in Account
        fromAcc->withdraw(amount);
        toAcc->deposit(amount);
        return true;
    }

    int getAccountCount() const {
        return static_cast<int>(accounts.size());
    }
};

//da controllare
bool isValidPin(const std::string &s) {
    if (s.size() != 4) return false;
    for (char c : s) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

//utilizzato per il login
Account* login(Bank &bank) {
    int accNum;
    std::string pin;

    cout << "\n--- Login ---\n";
    cout << "Enter account number: ";
    cin >> accNum;
    cout << "Enter PIN: ";
    cin >> pin;

    Account* acc = bank.findAccount(accNum);
    if (acc && acc->pin == pin) {
        cout << "Login successful!\n";
        return acc;
    }
    cout << "Invalid credentials.\n";
    return nullptr;
}

//utile
int generateAccountNumber() {
    static int seed = 1000; 
    seed++;
    return seed;
}

int main() {
    Bank bank;
    srand(time(NULL));

    cout << "<----------- Welcome to Trayner's Bank ------------->\n";

    // Create an initial account for demo purposes
    int newAccNum = generateAccountNumber();
    std::string userPin;
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

    // Login process
    int loginAccNum;
    std::string loginPin;
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

        //for checking if the account pin corresponds to the login pin
        userAccount = bank.findAccount(loginAccNum);
        if (userAccount == nullptr || userAccount->pin != loginPin) {
            loginAttempts++;
            cout << "Invalid account number or PIN. Attempt " << loginAttempts << "/" << MAX_LOGIN_ATTEMPTS << "\n";
            userAccount = nullptr;
        } else {
            cout << "Login successful!\n";
        }
    } while (userAccount == nullptr);

    // Transaction loop
    int choice;
    do {
        cout << "\n<----------- Menu ----------->\n";
        cout << "1) Check balance\n";
        cout << "2) Deposit money\n";
        cout << "3) Withdraw money\n";
        cout << "4) Transfer money\n"; // new option
        cout << "5) Create additional bank account\n";
        cout << "6) Change account (login)\n";
        cout << "7) Exit\n";
        cout << "8) Show number of accounts\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {

            case 1: {
                cout << "\n--- Account Balance ---\n";
                cout << "Your current balance: $" << userAccount->balance << "\n";//accede alla variabile dall'account dell'utente e mostra il bilancio prendendolo dalla funzione 
                break;
            }

            case 2: {
                cout << "\n--- Deposit Money ---\n";
                double amount;
                cout << "Enter amount to deposit: $";
                cin >> amount;
                userAccount->deposit(amount);//accede alla funzione deposit e prende la quantita depositata dall'utente 
                break;
            }

            case 3: {
                cout << "\n--- Withdraw Money ---\n";
                double amount;
                cout << "Enter amount to withdraw: $";
                cin >> amount;
                userAccount->withdraw(amount);
                break;
            }

            //when choosing this case it shows the available accounts where the money can be trasferred
            case 4:
            { // Transfer money
                cout << "\n--- Transfer Money ---\n";
                cout << "Available accounts for transfer:\n";

                // Show the list excluding the current account
                bank.showAvailableDestinations(userAccount->accountNumber);

                // Ask for the destination account
                int destAccNum;
                double amount;
                cout << "\nEnter the destination account number (0 to cancel): ";
                cin >> destAccNum;

                if (destAccNum == 0){
                    cout << "Transfer canceled.\n";
                    break;
                }

                // Verify if the entered account exists before proceeding
                Account *destAcc = bank.findAccount(destAccNum);
                if (destAcc == nullptr || destAcc->accountNumber == userAccount->accountNumber)
                {
                    cout << "Error: Destination account invalid or inexistent.\n";
                    break;
                }

                cout << "Enter the amount to transfer: $";
                cin >> amount;

                // Execute transfer and check if it was successful
                if (bank.transfer(userAccount->accountNumber, destAccNum, amount)){
                    cout << "Transfer completed successfully!\n";
                } else {
                    cout << "Transfer failed. Insufficient funds.\n";
                }
                break;
            }

            case 5: { // Create additional bank account
                int newAccNum = generateAccountNumber();
                std::string newPin;
                double newBalance;
                do {
                    cout << "Set PIN (exactly 4 digits): ";
                    cin >> newPin;
                } while (!isValidPin(newPin) || newPin == "1234" || newPin == "4321" || newPin == "1111" || newPin == "0000");
                cout << "Enter initial deposit: ";
                cin >> newBalance;

                bank.addAccount(Account(newAccNum, newPin, newBalance));
                cout << "New account created: " << newAccNum << "\n";
                break;
            }

            case 6:
            { // Change account (login) with limited attempts
                Account *previousUser = userAccount;
                int changeAttempts = 0;
                userAccount = nullptr;

                // Flag per capire se l'utente ha provato a inserire se stesso
                bool selfLoginError = false;

                while (!userAccount && changeAttempts < MAX_LOGIN_ATTEMPTS)
                {
                    userAccount = login(bank);

                    if (userAccount != nullptr)
                    {
                        // CONTROLLO ERRORE: L'utente ha inserito lo stesso conto attuale?
                        if (userAccount->accountNumber == previousUser->accountNumber)
                        {
                            cout << "\n[ERRORE]: Sei gia' autenticato con questo account number ("
                                 << previousUser->accountNumber << "). Azione annullata.\n";
                            selfLoginError = true;
                            break; // Interrompe il ciclo while immediatamente
                        }
                    }
                    else
                    {
                        // Se il login restituisce nullptr (credenziali errate)
                        changeAttempts++;
                        cout << "Login attempt " << changeAttempts << "/" << MAX_LOGIN_ATTEMPTS << " failed.\n";
                    }
                }

                // Se c'è stato l'errore di auto-login o se i tentativi sono falliti, ripristina il vecchio account
                if (!userAccount || selfLoginError)
                {
                    if (!selfLoginError)
                    {
                        cout << "Too many failed login attempts. Returning to main menu.\n";
                    }
                    userAccount = previousUser; // Paracadute: ripristina la sessione precedente
                }
                break;
            }

            case 7: { // Exit
                cout << "Thank you for using Trayner's Bank. Goodbye!\n";
                break;
            }

            case 8: {
                cout << "Current number of accounts: " << bank.getAccountCount() << "\n";
                break;
            }
            
            default:
                cout << "Invalid choice. Please try again.\n";
        }
  
      
    } while (choice != 7);

    return 0;
}