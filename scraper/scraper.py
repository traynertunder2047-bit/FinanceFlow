import sys #permette di leggere i dati che il programma c++ manda 
from curl_cffi import requests #posso  ignorare io headers perchè li specifica lui
from bs4 import BeautifulSoup
import urllib.parse #utilizzato per manipolare ulr, per convertire spazi e caratteri speciali inseriti dall'utente in un formato valido

def scrape_with_session(job_name):
    try:
         # Codifica la professione per l'URL (es. "ingegnere informatico" -> "ingegnere+informatico")
        job_encoded = urllib.parse.quote_plus(job_name.lower())#prende il nome del lavoro e lo trasformatto tutto in minuscolo e trasforma i caratteri speciali e i spazi (chef da cucina  -> chef+da+cucina)
    
        with requests.Session(impersonate="chrome110") as session:
            session.headers.update({
                "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
                "Accept-Language": "it-IT,it;q=0.9,en-US;q=0.8,en;q=0.7",
                "Sec-Fetch-Dest": "document",
                "Sec-Fetch-Mode": "navigate",
                "Sec-Fetch-Site": "same-origin",
                "Upgrade-Insecure-Requests": "1"
            })

            url_talent = f"https://it.talent.com/salary?job={job_encoded}"
        
            # Timeout a 10 secondi: se internet è lento, evita di bloccare il programma C++ all'infinito
            response = session.get(url_talent, timeout=10)
            if response.status_code == 200:
                soup = BeautifulSoup(response.text, 'html.parser')  
                # Selettore HTML per la cifra dello stipendio medio su Talent.com
                salary_card = soup.find('div', class_='time-card-main-amount')
                if salary_card:
                    return f"{salary_card.text.strip()} (source: talent.com)"
                

            #if the site still dose not accept the script -> response = session.get(url_jobbydoo, headers={"Referer": "https://www.jobbydoo.it/"}, timeout=10)
            #Sito di riserva: Tenta lo scraping da jobbydoo.it
            url_jobbydoo= f"https://www.jobbydoo.it/stipendio/{job_encoded}"
            response = session.get(url_jobbydoo, timeout=10)
            
            if response.status_code == 200:
                soup = BeautifulSoup(response.text, 'html.parser')
                # Struttura HTML di Jobbydoo (usa una classe diversa)
                salary_badge = soup.find('div', class_='salary-value')
                if salary_badge:
                    return f"{salary_badge.text.strip()} (source: jobbydoo.com)"
                
        return None
    except Exception:
        return None


if __name__ == "__main__":
    # 1. Controlla se il C++ ha passato l'argomento (il nome del lavoro)
    if len(sys.argv) > 1:
        # Unisce gli argomenti in caso di nomi composti (es. ["Ingegnere", "Informatico"] -> "Ingegnere Informatico")
        work = " ".join(sys.argv[1:])
        
        # 2. Prova la prima sorgente
        result = scrape_with_session(work)
        
           
        if not result:
            result = "Stipendio non trovato (struttura del sito variata)."
            
        # 5. STAMPA FINALE: Il C++ catturerà esattamente questa riga tramite il terminale
        print(result)
    else:
        print("Errore: Nessuna professione specificata da C++.")



"""
import sys #permette di leggere i dati che il programma c++ manda 
import requests
from bs4 import BeautifulSoup
import urllib.parse #utilizzato per manipolare ulr, per convertire spazi e caratteri speciali inseriti dall'utente in un formato valido

def scrape_talent(job_name):
    #Tenta lo scraping dello stipendio medio da it.talent.com
    try:
        # Codifica la professione per l'URL (es. "ingegnere informatico" -> "ingegnere+informatico")
        job_encoded = urllib.parse.quote_plus(job_name.lower()) #prende il nome del lavoro e lo trasformatto tutto in minuscolo e trasforma i caratteri speciali e i spazi (chef da cucina  -> chef+da+cucina)
        url = f"https://it.talent.com/salary?job={job_encoded}" #è una stringa formattata e permette di inserire variabili nelle {} f"https://it.talent.com/salary?job={idraulico}"
        
        # Simula un browser reale per evitare blocchi (User-Agent)
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
            "Accept-Language": "it-IT,it;q=0.9,en-US;q=0.8,en;q=0.7"
        }
        
        # Timeout a 5 secondi: se internet è lento, evita di bloccare il programma C++ all'infinito
        response = requests.get(url, headers=headers, timeout=10)
        if response.status_code != 200:
            return None
        
        soup = BeautifulSoup(response.text, 'html.parser')
        
        # Selettore HTML per la cifra dello stipendio medio su Talent.com
        salary_card = soup.find('div', class_='time-card-main-amount')
        
        if salary_card:
            return f"{salary_card.text.strip()} (Fonte: Talent.com)"
        return None
    except Exception:
        return None


def scrape_jobbydoo(job_name):
    #Sito di riserva: Tenta lo scraping da jobbydoo.it
    try:
        job_encoded = urllib.parse.quote_plus(job_name.lower())
        url = f"https://www.jobbydoo.it/stipendio/{job_encoded}"
        
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"
        }
        
        response = requests.get(url, headers=headers, timeout=5)
        if response.status_code != 200:
            return None
            
        soup = BeautifulSoup(response.text, 'html.parser')
        
        # Struttura HTML di Jobbydoo (usa una classe diversa)
        salary_badge = soup.find('div', class_='salary-value')
        
        if salary_badge:
            return f"{salary_badge.text.strip()} all'anno (Fonte: Jobbydoo)"
        return None
    except Exception:
        return None
    

if __name__ == "__main__":
    # 1. Controlla se il C++ ha passato l'argomento (il nome del lavoro)
    if len(sys.argv) > 1:
        # Unisce gli argomenti in caso di nomi composti (es. ["Ingegnere", "Informatico"] -> "Ingegnere Informatico")
        professione = " ".join(sys.argv[1:])
        
        # 2. Prova la prima sorgente
        risultato = scrape_talent(professione)
        
        # 3. Se la prima fallisce (ritorna None), passa alla seconda di riserva
        if not risultato:
            risultato = scrape_jobbydoo(professione)
            
        # 4. Se entrambe falliscono o i siti hanno cambiato radicalmente l'HTML
        if not risultato:
            risultato = "Stipendio non trovato (struttura del sito variata)."
            
        # 5. STAMPA FINALE: Il C++ catturerà esattamente questa riga tramite il terminale
        print(risultato)
    else:
        print("Errore: Nessuna professione specificata da C++.")



"""