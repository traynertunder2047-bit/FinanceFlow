#include <iostream>
#include <string>
#include <cstdlib> // per stof
#include <cstdio>  // per _popen e _pclose
#include <vector>
#include <cmath>
#include <sstream>
using namespace std;


bool containsUnsafeCharacters(string &input) {
    return input.find_first_of("\"`$;|&<>\\") != string::npos;
}

// Esegue un comando esterno e ne cattura SOLO lo stdout (esattamente come
// fa _popen/popen con modalità "r"). Lo stderr del processo figlio NON
// viene catturato qui, quindi eventuali script Python possono stampare
// liberamente log/debug su stderr senza sporcare l'output che leggiamo.
string eseguiComandoECatturaOutput(const string &command) {
    string output;
    char buffer[128];

    #ifdef _WIN32
        // Bug noto di cmd.exe: se il comando inizia già con una stringa tra
        // virgolette (es. il percorso dell'eseguibile Python nel venv),
        // cmd.exe toglie la prima e l'ultima virgoletta dell'intera riga,
        // rompendo la corrispondenza delle virgolette successive e
        // spezzando gli argomenti con spazi (es. "backend engineer" diventa
        // solo "backend"). La correzione standard è avvolgere l'intero
        // comando in un'ulteriore coppia di virgolette esterne.
        string comandoFinale = "\"" + command + "\"";
        FILE *pipe = _popen(comandoFinale.c_str(), "r");
    #else
        FILE *pipe = popen(command.c_str(), "r");
    #endif

    if (!pipe) {
        return "";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    #ifdef _WIN32
        _pclose(pipe);
    #else
        pclose(pipe);
    #endif

    return output;
}


float estraiPrimoNumero(const string &testo) {
    istringstream inputStream(testo);
    string primoToken;
    if (inputStream >> primoToken) {
        try {
            return stof(primoToken);
        } catch (const exception&) {
            return 0.0f;
        }
    }
    return 0.0f;
}


string trovaEseguibilePython() {
    #ifdef _WIN32
        string pythonVenv = "venv\\Scripts\\python.exe";
    #else
        string pythonVenv = "venv/bin/python3";
    #endif

    FILE *test = fopen(pythonVenv.c_str(), "r");
    if (test != nullptr) {
        fclose(test);
        return "\"" + pythonVenv + "\"";
    }

    // Fallback: nessun venv trovato accanto all'eseguibile, usa il Python
    // di sistema disponibile nel PATH del terminale corrente.
    #ifdef _WIN32
        return "python";
    #else
        return "python3";
    #endif
}


int main() {
    string job_name, command, pythonOutput, country_name, financialObjective;
    double survivalExpense, leisureExpense, rentExpense, totalExpenses, savingsCapacity, totalPercentage = 0.0, monthlyAllocatedMoney, monthsNeeded;
    float tax, grossAnnualSalary, monthlySalary;
    int numObjectives;

    cout << "Enter the name of the job you want to analyse: ";
    getline(cin, job_name);
    cout << "Enter the name of the country for the job's salary you're interested: ";
    getline(cin, country_name);

    if (containsUnsafeCharacters(job_name) || containsUnsafeCharacters(country_name)) {
        cout << "[ERROR]: Input contains invalid characters.\n";
        return 1;
    }

    
    cout << "[Python is scraping jobadatalake API for job]\n";

    string pythonExe = trovaEseguibilePython();
    command = pythonExe + " scraper.py \"" + job_name + "\" \"" + country_name + "\"";
    /* _popen fa 3 cose contemporaneamente: crea un nuovo processo in background
       (terminale nascosto), esegue il comando contenuto in command, apre un
       canale di comunicazione "pipe" tra lo script python e c++, "r" sta per
       read e permette al C++ di leggere tutto quello che viene messo in
       print dallo scraper. c_str() trasforma la string in una stringa del
       linguaggio C richiesta dalla funzione _popen. */

    pythonOutput = eseguiComandoECatturaOutput(command);

    if (pythonOutput.empty()) {
        cout << "[ERROR]: Could not execute the scraper script.\n";
        return 1;
    }

    grossAnnualSalary = estraiPrimoNumero(pythonOutput);

    cout << "Gross annual salary received: " << grossAnnualSalary << "\n";

    cout << "[Python is calculating income tax for " << country_name << "]\n";

    ostringstream ralStream;
    ralStream << grossAnnualSalary;

    command = pythonExe + " calcola_tasse.py \"" + country_name + "\" \"" + ralStream.str() + "\"";

    string taxOutput = eseguiComandoECatturaOutput(command);

    if (taxOutput.empty()) {
        cout << "[ERROR]: Could not execute the tax calculation script. Assuming 0 tax.\n";
        tax = 0.0f;
    } else {
        tax = estraiPrimoNumero(taxOutput);
    }

    grossAnnualSalary -= tax;
    cout << "With taxes depending on the annual income the salary will be " << grossAnnualSalary << " with a tax of " << tax << "\n";

    monthlySalary = grossAnnualSalary / 12;
    cout << "Estimated monthly salary: " << monthlySalary << " with taxes " << "\n";


    
    cout << "Set up your monthly spending profile: ";
    cout << "1. Enter the monthly expense of your housing/rent/morgage: ";
    cin >> rentExpense;
    cout << "2. Enter the monthly expense for survival: ";
    cin >> survivalExpense;
    cout << "3. Enter the monthly expense for transport/leisure: ";
    cin >> leisureExpense;


    
    totalExpenses = rentExpense + survivalExpense + leisureExpense;
    cout << "ANALYSIS RESULT: \n";
    cout << "Total monthly expenses: " << totalExpenses << "\n";

    savingsCapacity = monthlySalary - totalExpenses;
    cout << "Savings capacity: " << savingsCapacity << "\n";
    


    cout << "Objectives planning: ";
    if (savingsCapacity > 0) {
        cout << "\n=========================================\n";
        cout << "          OBJECTIVES PLANNING            \n";
        cout << "=========================================\n";
        cout << "How many financial objectives do you want to plan? ";
        cin >> numObjectives;

        if (numObjectives <= 0) {
            cout << "[WARNING]: The number of objectives must be positive. Skipping planning.\n";
        } else {
            vector<string> objNames(numObjectives);
            vector<double> objCosts(numObjectives);
            vector<double> objPercentages(numObjectives);

            for (int i = 0; i < numObjectives; i++) {
                cout << "\n--- Objective #" << i + 1 << " ---\n";
                cout << "Enter the name of the objective: ";
                cin.ignore(); // Pulisce il buffer prima di getline
                getline(cin, objNames[i]);

                cout << "Enter the total cost: ";
                cin >> objCosts[i];

                cout << "What % of your monthly savings do you want to allocate here? ";
                cin >> objPercentages[i];

                totalPercentage += objPercentages[i];
            }

            
            if (totalPercentage > 100.0) {
                cout << "\n[WARNING]: The total allocation (" << totalPercentage << "%) exceeds 100% of your savings!\n";
                cout << "Please restart the planning and balance your percentages properly.\n";
            } else {
                if (totalPercentage < 100.0) {
                    cout << "\n[Note]: You allocated " << totalPercentage
                         << "% of your savings. The remaining " << (100.0 - totalPercentage)
                         << "% will go into generic savings.\n";
                }

                cout << "\n=========================================\n";
                cout << "            ALLOCATION REPORT            \n";
                cout << "=========================================\n";

                for (int i = 0; i < numObjectives; i++) {
                    monthlyAllocatedMoney = savingsCapacity * (objPercentages[i] / 100.0);

                    if (monthlyAllocatedMoney <= 0.0) {
                        cout << "* '" << objNames[i] << "' (Cost: " << objCosts[i] << "):\n";
                        cout << "- Monthly allocation is zero or negative, so the target cannot be estimated.\n\n";
                        continue;
                    }

                    monthsNeeded = objCosts[i] / monthlyAllocatedMoney;

                    cout << "* '" << objNames[i] << "' (Cost: " << objCosts[i] << "):\n";
                    cout << "- Monthly allocation: " << monthlyAllocatedMoney << " (" << objPercentages[i] << "%)\n";
                    cout << "- Estimated time to achieve: " << ceil(monthsNeeded) << " months\n\n";
                }
            }
        }
    }

    return 0;
}