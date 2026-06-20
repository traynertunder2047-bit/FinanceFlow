#include <iostream>
#include <string>

using namespace std;


int main() {
    string job_name;

    cout << "Enter the name the job you want to analyse: ";
    cin >> job_name;
    /*TUTTA LA STRUTTURA PER LO SCRAPER */

    cout << "[Python is scraping jobadatalake API for job]";

    cout << "Market values found: ";

    cout << "Annual salary(RAL) -> ";
    cout << "Estimated monthly salary: ";
    /*TUTTA LA STRUTTURA PER IL SALARIO ANNUALE E IL CALCOLO DEL SALARIO MENSILE ESTIMATO*/
    cout << "Set up your monthly spending profile: ";
    cout << "1. Enter the expense of your housing/rent/morgage: ";
    cout << "2. Enter the expense for survival: ";
    cout << "3. Enter the expense for transport/leisure: ";
    /*TUTTA LA STRUTTURA PER LE SPESE  */

    cout << "ANALYSIS RESULT: ";
    cout << "Total monthly expenses: ";
    cout << "Savings capacity: ";
    
    cout << "Objectives planing: ";

    cout << "Obiettivo risparmio";
    cout << "With your actual profile you will achive your objective int tot months";
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