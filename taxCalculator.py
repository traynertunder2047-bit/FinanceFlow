import os, sys, json

from pdfChapterExtractor import estrai_testo_capitolo
from pdfParamExtractor import estrai_parametri_da_pdf
from scraper import cerca_parametri_fiscali, calcola_tassa_da_scaglioni
from cacheTaxParams import leggi_da_cache, scrivi_in_cache, svuota_cache

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MAPPA_PDF_PATH = os.path.join(SCRIPT_DIR, "country_chapters.json")
PDF_PATH = os.path.join(SCRIPT_DIR, "TaxingWages2026.pdf")


def _log(messaggio: str) -> None:
    print(messaggio, file=sys.stderr)


def _paese_presente_nel_pdf(paese: str) -> bool:
    if not os.path.exists(MAPPA_PDF_PATH):
        return False
    with open(MAPPA_PDF_PATH, "r", encoding="utf-8") as f:
        mappa = json.load(f)

    chiave = paese.strip().lower()
    if chiave in mappa:
        return True
    candidati = [k for k in mappa if chiave in k or k in chiave]
    return len(candidati) == 1


def calcola_tasse(paese: str, ral: float, forza_refresh: bool = False) -> dict:
    """
      - "tassa": float, l'imposta calcolata
      - "fonte": str, quale strategia è stata usata
                 ("cache_pdf" | "cache_web" | "pdf" | "web")
      - "parametri": dict, gli scaglioni/parametri usati per il calcolo
    """
    if forza_refresh:
        _log(f"Refresh forzato: ignoro (ed elimino) l'eventuale cache per '{paese}'.")
        svuota_cache(paese)
    else:
        entry_cache = leggi_da_cache(paese)
        if entry_cache is not None:
            _log(
                f"'{paese}' trovato in cache (salvato il {entry_cache['salvato_il']}, "
                f"fonte originale: {entry_cache['fonte_originale']}). Nessuna chiamata AI."
            )
            return {
                "tassa": calcola_tassa_da_scaglioni(ral, entry_cache["parametri"]),
                "fonte": f"cache_{entry_cache['fonte_originale']}",
                "parametri": entry_cache["parametri"],
            }

    # --- Cache assente (o refresh forzato): serve una chiamata AI ---
    if _paese_presente_nel_pdf(paese) and os.path.exists(PDF_PATH):
        _log(f"'{paese}' non in cache. Trovato nell'indice del PDF: estraggo gli scaglioni dal capitolo dedicato.")
        testo_capitolo = estrai_testo_capitolo(paese, PDF_PATH)
        parametri = estrai_parametri_da_pdf(paese, testo_capitolo)
        fonte = "pdf"
    else:
        _log(f"'{paese}' non in cache e non nell'indice del PDF. Cerco gli scaglioni sul web...")
        parametri = cerca_parametri_fiscali(paese)
        fonte = "web"

    scrivi_in_cache(paese, parametri, fonte)
    _log(f"Parametri per '{paese}' salvati in cache per le prossime esecuzioni.")

    tassa = calcola_tassa_da_scaglioni(ral, parametri)

    return {
        "tassa": tassa,
        "fonte": fonte,
        "parametri": parametri,
    }


def main():
    argomenti = sys.argv[1:]
    forza_refresh = "--refresh" in argomenti
    argomenti = [a for a in argomenti if a != "--refresh"]

    if len(argomenti) != 2:
        _log("Uso: python calcola_tasse.py <country> <gross_annual_salary> [--refresh]")
        print("0.00")
        sys.exit(1)

    paese = argomenti[0].strip()

    try:
        ral = float(argomenti[1])
    except ValueError:
        _log(f"ERROR: '{argomenti[1]}' non è un numero valido per la RAL.")
        print("0.00")
        sys.exit(1)

    try:
        risultato = calcola_tasse(paese, ral, forza_refresh=forza_refresh)
    except Exception as e:
        _log(f"ERROR durante il calcolo: {e}")
        print("0.00")
        sys.exit(1)

    # --- Log diagnostico completo, SOLO su stderr ---
    _log(f"Fonte usata: {risultato['fonte']}")
    _log("Scaglioni/parametri usati:")
    _log(json.dumps(risultato["parametri"], indent=2, ensure_ascii=False))

    credito = risultato["parametri"].get("tax_credit", 0) or 0
    if credito > 0:
        _log(f"Credito d'imposta applicato: {credito:,.2f}")
    else:
        _log("Nessun credito d'imposta applicato (non trovato o non riportato come valore semplice/affidabile).")

    _log(f"Salario lordo:         {ral:,.2f}")
    _log(f"Tassa calcolata:       {risultato['tassa']:,.2f}")
    _log(f"Salario netto stimato: {ral - risultato['tassa']:,.2f}")
    _log(
        "ATTENZIONE: calcolo semplificato basato su scaglioni base "
        "(+ un eventuale credito d'imposta semplice a importo fisso, se "
        "trovato in modo affidabile). NON include addizionali regionali/"
        "comunali, bonus complessi o crediti che dipendono dal livello di "
        "reddito o da altre condizioni. È una stima, non un valore ufficiale."
    )

    # --- UNICA riga su stdout: il numero che il C++ deve leggere ---
    print(f"{risultato['tassa']:.2f}")


if __name__ == "__main__":
    main()