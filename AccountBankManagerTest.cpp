/*Cose da cambiare
1-- dividere le classi in file diversi per rendere il codice piu organizzato(verso la fine), momentaneamente preferisco tenerle in questo modo anche se è un casino
2-- prima di creare tutta la UI migliorare la grafica da terminale rimediando alla continua ripetizione del menu che porta a rendere il programma un po più confusionario{{{DA RISOLVERE}}}
4-- quando chiede di depositare il denaro questo viene depositato senza alcuna traccia della provenienza, rimediare magari con l'aggiunta di un modo per guadagnare 
denaro, per esempio con la compravendita di azioni e criptovalute
*/

/*----IDEE----
1)) Momentaneamente il programma mi sembra un po vuoto, l'idea era di aggiungere una serie di funzioni e classi che rappresentano account bancari di diverse aziende, l'utente
riceverebbe un salario mensile che puo decidere di spendere in diversi modi, in alcuni casi tipo mercati virtuali per la compravendita di azioni e criptovalute,
(il problema è che mi sembra un po fuori posto), 

2)) Si puo aggiungere insieme all'idea dei diversi account bancari di aziende anche la gestione di prestiti e mutui, anche debiti.()

3)) Pensavo di creare un vero e proprio wallet virtuale, tipo quelli utilizzati per le cripotvalute, non so esattamente cosa aggiungere per renderlo più completo e aderente all'idea iniziale
si poteva dividere in 3 parti con: 1)Account -> tutta la parte bilancio, deposito, prelievo, 2)Wallet -> tutte le spese e i guadagni, magari con tanto di cronologia e rapporti
gudagni/spese, 3)Market -> compravendita di azioni?? / criptovalute (può essere un po fuori posto), 4)Settings -> tutte le impostazioni -> cambio pin, account ,creazione account, numero acc.,  5) Exit.


4)) Pensavo di aggiungere diverse cose: --1-- Nome per ogni account(Stringa che setto ToLowerCase), --2-- Incremento numero di caratteri del pin 4 -> 8 + aggiunta di un PIN con lettere, caratteri speciali e numero per 
renderlo più complesso, --3-- Account per minorenni con limiti per i depositi, prelievo, spese, traferimenti, --4-- Notifiche sulle cose compiute tipo un prelievo(?).

*/

/* ------ NOMI ------
 FlowBank (professionale, semplice e facile da ricordare)
 FinanceFlow (suggerisce un flusso di denaro e finanza, più grande in dimensioni di programma e codice)
 NexBank (suggerisce un'idea di banca del futuro, innovativa e tecnologica)

 le migliori sono FinanceFlow e NexBank per momento
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

//LOGIN
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

//NUMERO ACCOUNT 
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
        cout << "7) Show number of accounts\n";
        cout << "8) Exit\n";
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


            //Possibilie aggiunta/modifica codice verifica sopra
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
                cout << "\n--- Change Account ---\n";
                Account *previousUser = userAccount;
                int changeAttempts = 0;
                userAccount = nullptr;

                // Flag per capire se l'utente ha provato a inserire se stesso
                bool selfLoginError = false;

                while (!userAccount && changeAttempts < MAX_LOGIN_ATTEMPTS) {

                    cout << "Available accounts for login:\n";
                    bank.showAvailableDestinations(previousUser->accountNumber);
                    //mi serve previousUser->accountNumber per mostrare solo gli account diversi da quello attualmente in uso, non userAccount che mi avrebbe mostrate quello attuale

                    userAccount = login(bank);//stesso login del login iniziale(chiama la funzione login)
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
                cout << "\n--- Account Count ---\n";
                cout << "Current number of accounts: " << bank.getAccountCount() << "\n";
                break;
            }

            case 8: {
                cout << "\n--- Exit ---\n";
                cout << "Thank you for using Trayner's Bank. Goodbye!\n";
                break;
            }
            
            default:
                cout << "\n--- Error ---\n";
                cout << "Invalid choice. Please try again.\n";
        }
  
      
    } while (choice != 8);

    return 0;
}