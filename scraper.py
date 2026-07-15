import os, re, json
from dotenv import load_dotenv
from google import genai
from google.genai import types


def cerca_parametri_fiscali(paese: str) -> dict:
    load_dotenv()
    API_KEY = os.environ.get("GEMINI_API_KEY")
    if not API_KEY:
        raise RuntimeError("GEMINI_API_KEY non trovata nell'ambiente")

    client = genai.Client(api_key=API_KEY)

    prompt = f"""
    Search the web for the OFFICIAL, CURRENT personal income tax brackets
    (national/federal level) for a single employee with no dependents in:
    {paese}

    Prefer official government sources (tax agency, ministry of finance)
    over blogs or calculators. Look for the most recent tax year available.

    IMPORTANT: You must NOT perform any tax calculation yourself. You must
    NOT apply the brackets to any salary. You must NOT sum, multiply, or
    compute any final tax amount. Your ONLY job is to find and report the
    raw bracket structure as published by the tax authority.

    Also check whether a SIMPLE, FIXED-AMOUNT basic tax credit exists
    (a single flat number subtracted directly from the tax liability,
    e.g. "a basic tax credit of X currency units applies to all
    taxpayers"). Only report this if it is stated as one clear fixed
    number you are confident about. If the tax credit system is more
    complex (income-dependent, phases out gradually, has multiple
    conditions, or you are not fully sure of the exact figure), set
    "tax_credit" to 0 rather than guessing - an omitted credit is far
    safer than a wrong one.

    Respond with ONLY a valid JSON object (no markdown code fences, no
    explanation before or after) with EXACTLY this structure:

    {{
      "country": "{paese}",
      "year": <int, the tax year these brackets refer to>,
      "currency": "<ISO currency code, e.g. EUR, USD>",
      "standard_deduction": <float, 0 if none found - a basic tax-free
         allowance/no-tax area applied BEFORE brackets, if any>,
      "tax_brackets": [
        {{"upper_bound": <float or null for the top open-ended bracket>, "rate": <float, e.g. 0.23 for 23%>}}
      ],
      "tax_credit": <float, 0 if none found or too complex/uncertain to
         state as a single fixed number - a simple flat amount subtracted
         directly from the final tax liability>,
      "sources": ["<url1>", "<url2>"],
      "notes": "<any important caveat, e.g. regional variation, or a
         mention of tax credits that exist but were too complex to
         quantify simply, or empty string>"
    }}

    The tax_brackets array must be ordered from lowest to highest, and
    upper_bound values must be in the stated currency (not a percentage).
    """

    response = client.models.generate_content(
        model="gemini-3.5-flash",
        contents=[prompt],
        config=types.GenerateContentConfig(
            tools=[types.Tool(google_search=types.GoogleSearch())],
            temperature=0,
        ),
    )

    testo = response.text.strip()

    # L'AI a volte avvolge il JSON in ```json ... ``` nonostante l'istruzione
    # di non farlo: lo ripuliamo per sicurezza prima di fare il parse.
    testo_pulito = re.sub(r"^```(json)?|```$", "", testo, flags=re.MULTILINE).strip()

    try:
        dati = json.loads(testo_pulito)
    except json.JSONDecodeError as e:
        raise RuntimeError(
            f"L'AI non ha restituito un JSON valido. Risposta grezza:\n{testo}"
        ) from e

    return dati


def calcola_tassa_da_scaglioni(salario_lordo: float, parametri: dict) -> float:
    deduzione = parametri.get("standard_deduction", 0) or 0
    imponibile = max(0.0, salario_lordo - deduzione)

    scaglioni = parametri["tax_brackets"]

    imposta = 0.0
    soglia_precedente = 0.0
    for fascia in scaglioni:
        soglia_superiore = fascia["upper_bound"]
        aliquota = fascia["rate"]

        if soglia_superiore is None:
            # ultima fascia, senza limite superiore
            if imponibile > soglia_precedente:
                imposta += (imponibile - soglia_precedente) * aliquota
            break

        if imponibile <= soglia_precedente:
            break

        quota_in_fascia = min(imponibile, soglia_superiore) - soglia_precedente
        imposta += quota_in_fascia * aliquota
        soglia_precedente = soglia_superiore

    # --- Credito d'imposta semplice (opzionale, con fallback sicuro a 0) ---
    credito_grezzo = parametri.get("tax_credit", 0)
    try:
        credito = float(credito_grezzo) if credito_grezzo is not None else 0.0
        if credito < 0:
            credito = 0.0
    except (TypeError, ValueError):
        # Valore non numerico/malformato: ignoriamo il credito invece di
        # far crashare il calcolo o rischiare un numero inventato.
        credito = 0.0

    imposta_netta = max(0.0, imposta - credito)

    return round(imposta_netta, 2)


def main():
    paese = input("Enter the name of the country: ").strip()
    ral = float(input("Enter the gross annual salary (RAL): ").strip())

    print("\nCerco gli scaglioni fiscali ufficiali sul web...\n")
    parametri = cerca_parametri_fiscali(paese)

    print("--- Parametri trovati dall'AI (nessun calcolo, solo dati grezzi) ---")
    print(json.dumps(parametri, indent=2, ensure_ascii=False))
    print("---------------------------------------------------------------------\n")

    if parametri.get("notes"):
        print(f"NOTA dell'AI: {parametri['notes']}\n")

    credito = parametri.get("tax_credit", 0) or 0
    if credito > 0:
        print(f"Credito d'imposta applicato: {credito:,.2f} {parametri.get('currency', '')}\n")
    else:
        print("Nessun credito d'imposta applicato (non trovato o non riportato come valore semplice/affidabile).\n")

    tassa = calcola_tassa_da_scaglioni(ral, parametri)
    netto = ral - tassa

    print(f"Salario lordo:        {ral:>12,.2f} {parametri.get('currency', '')}")
    print(f"Imposta calcolata:    {tassa:>12,.2f} {parametri.get('currency', '')}")
    print(f"Salario netto stimato:{netto:>12,.2f} {parametri.get('currency', '')}")
    print(f"\nFonti usate dall'AI per trovare gli scaglioni:")
    for fonte in parametri.get("sources", []):
        print(f"  - {fonte}")

    print(
        "\nATTENZIONE: calcolo semplificato basato su scaglioni base "
        "(+ un eventuale credito d'imposta semplice a importo fisso, se "
        "trovato in modo affidabile). NON include addizionali regionali/"
        "comunali, bonus complessi o crediti che dipendono dal livello di "
        "reddito o da altre condizioni. È una stima, non un valore ufficiale."
    )