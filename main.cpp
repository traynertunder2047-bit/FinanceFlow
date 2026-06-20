#include <iostream>
#include <string>
#include <cstdlib>// per stof
#include <cstdio>// per _popen e _pclose

using namespace std;


int main() {
    string job_name, command, pythonOutput;
    double monthlySalary, survivalExpense, leisureExpense, rentExpense, totalExpenses, savingsCapacity;
    char buffer[128];//crea una memoria temporanea utilizzata come contenitore per i caratteri mandati dallo scraper
    cout << "Enter the name the job you want to analyse: ";
    cin >> job_name;
    cout << "[Python is scraping jobadatalake API for job]";
    /*TUTTA LA STRUTTURA PER LO SCRAPER */
    command = "Python scraper.py \"" + job_name + "\"";//le \" si usano per inserire le virgolette attorno al nome del lavoro in modo che python riceva una string unica
    FILE* pipe = _popen(command.c_str(), "r");
    /*_popen fa 3 cose contemporaneamente:  crea un nuovo processo in background(terminale nascosto), esegue il comando contenuto in command, apre un canale di comunicazione
    "pipe" tra lo script python e c++, "r" sta per read e permette allo script in c++ di leggere tutto quello che viene messo in print dallo scraper,
    c_str() trasforma la string in una stringa del linguaggio C richiesta dalla funzione _popen */
    if (!pipe) {
        cout << "[ERROR]: Could not execute the scraper script.\n";
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {/*buffer è il contenitore che conterrà i caratteri mandati dallo scraper, sizeof è il massimo di caratteri che può raccogliere
        pipe è il canale da dove li deve prendere*/
        pythonOutput += buffer;
    }
 
    _pclose;    
    float ral = stof(pythonOutput);
    cout << "RAL recived: " << ral << "\n";//alternative cout << "Annual salary(RAL) -> ";

    cout << "Market values found: ";

    monthlySalary = ral / 12;
    cout << "Estimated monthly salary: " << monthlySalary << "\n";
    
    /*TUTTA LA STRUTTURA PER IL SALARIO ANNUALE E IL CALCOLO DEL SALARIO MENSILE ESTIMATO*/
    cout << "Set up your monthly spending profile: ";
    
    cout << "1. Enter the monthly expense of your housing/rent/morgage: ";
    cin >> rentExpense;
    cout << "2. Enter the monthly expense for survival: ";
    cin >> survivalExpense;
    cout << "3. Enter the monthly expense for transport/leisure: ";
    cin >> leisureExpense;
    /*TUTTA LA STRUTTURA PER LE SPESE  */

    totalExpenses = rentExpense + survivalExpense + leisureExpense;
    cout << "ANALYSIS RESULT: ";
    cout << "Total monthly expenses: " << totalExpenses << "\n";

    savingsCapacity = monthlySalary - totalExpenses;
    cout << "Savings capacity: ";
    //COTNROLLO PER VERIFICARE SE L'UTENTE PUO' PASSARE ALLA PIANIFICAZIONE DEGLI OBIETTIVI
    cout << "Objectives planing: ";
    cout << "Savings objective";
    cout << "With your actual profile you will achive your objective int tot months";

    cout << "Investement simulation";
    return 0;
}





































/* case 6: { // --- INTEGRAZIONE SCRAPER PYTHON ---
                    string jobName;
                    cout << "\nEnter the job profile to research (e.g., ingegnere informatico): ";
                    cin.ignore(); // Pulisce il buffer prima di getline
                    getline(cin, jobName); // Permette di leggere anche i nomi con spazi

                    cout << "[Bank-System]: Fetching salary data via Python Scraper, please wait...\n";

                    // Costruiamo il comando di sistema (es: python scraper.py "ingegnere informatico")
                    string command = "python scraper.py \"" + jobName + "\"";

                    // Apriamo una pipe per catturare l'output dello script Python
                    FILE* pipe = _popen(command.c_str(), "r");
                    if (!pipe) {
                        cout << "[ERROR]: Could not execute the scraper script.\n";
                        break;
                    }

                    char buffer[128];
                    string pythonOutput = "";

                    // Leggiamo riga per riga l'output stampato da Python
                    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                        pythonOutput += buffer;
                    }

                    // Chiudiamo la pipe di sistema
                    _pclose(pipe);

                    // Mostriamo il risultato finale all'utente dell'app bancaria
                    cout << "\n-------------------------------------\n";
                    cout << "SALARY INSIGHT FOR: " << jobName << "\n";
                    cout << "Result: " << pythonOutput;
                    cout << "-------------------------------------\n";
                    break;
                } */