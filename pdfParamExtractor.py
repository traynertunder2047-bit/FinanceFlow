import os, re, json
from dotenv import load_dotenv
from google import genai
from google.genai import types


def estrai_parametri_da_pdf(paese: str, testo_capitolo: str) -> dict:
    """
    {
        "country": str,
        "year": int,
        "currency": str,
        "standard_deduction": float,
        "tax_brackets": [{"upper_bound": float | None, "rate": float}, ...],
        "tax_credit": float,   # 0 se assente/non affidabile come valore singolo
        "sources": [str, ...],
        "notes": str
    }
    """
    load_dotenv()
    API_KEY = os.environ.get("GEMINI_API_KEY")
    if not API_KEY:
        raise RuntimeError("GEMINI_API_KEY non trovata nell'ambiente")

    client = genai.Client(api_key=API_KEY)

    prompt = f"""
    Read the following document excerpt (Taxing Wages 2026, {paese} chapter)
    and extract ONLY the raw personal income tax bracket structure
    (national/central government level) for a single employee with no
    dependents.

    DOCUMENT EXCERPT:
    {testo_capitolo}

    IMPORTANT: You must NOT perform any tax calculation yourself. You must
    NOT apply the brackets to any salary. You must NOT sum, multiply, or
    compute any final tax amount. Your ONLY job is to find and report the
    raw bracket structure exactly as stated in the document.

    Also check whether the document mentions a SIMPLE, FIXED-AMOUNT basic
    tax credit (a single flat number subtracted directly from the tax
    liability, applying to all/most taxpayers regardless of income level).
    Only report this if it is stated as one clear fixed number you are
    confident about. If the tax credit system described in the document is
    more complex (income-dependent, phases out gradually across brackets,
    has multiple conditions, or you are not fully sure of the exact
    figure), set "tax_credit" to 0 rather than guessing - an omitted
    credit is far safer than a wrong one. You may mention the existence
    of a more complex credit in "notes" without quantifying it.

    Respond with ONLY a valid JSON object (no markdown code fences, no
    explanation before or after) with EXACTLY this structure:

    {{
      "country": "{paese}",
      "year": <int, the tax year these brackets refer to, as stated in the document>,
      "currency": "<ISO currency code, e.g. EUR, USD>",
      "standard_deduction": <float, 0 if none found - a basic tax-free
         allowance/no-tax area applied BEFORE brackets, if any>,
      "tax_brackets": [
        {{"upper_bound": <float or null for the top open-ended bracket>, "rate": <float, e.g. 0.23 for 23%>}}
      ],
      "tax_credit": <float, 0 if none found or too complex/uncertain to
         state as a single fixed number - a simple flat amount subtracted
         directly from the final tax liability>,
      "sources": ["Taxing Wages 2026 (OECD), {paese} chapter"],
      "notes": "<any important caveat, or empty string>"
    }}

    The tax_brackets array must be ordered from lowest to highest, and
    upper_bound values must be in the stated currency (not a percentage).
    """

    response = client.models.generate_content(
        model="gemini-3.5-flash",
        contents=[prompt],
        config=types.GenerateContentConfig(temperature=0),
    )

    testo = response.text.strip()
    testo_pulito = re.sub(r"^```(json)?|```$", "", testo, flags=re.MULTILINE).strip()

    try:
        dati = json.loads(testo_pulito)
    except json.JSONDecodeError as e:
        raise RuntimeError(
            f"L'AI non ha restituito un JSON valido. Risposta grezza:\n{testo}"
        ) from e

    return dati