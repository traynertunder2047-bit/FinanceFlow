import os, re, json
from dotenv import load_dotenv
from google import genai
from google.genai import types
try:
    from curl_cffi import requests
except ImportError:
    import requests

from app.currencyConverter import usd_to_currency

SENIORITY_MAP = {
    "Senior": ["senior"],
    "Mid level": ["mid level", "mid-level"],
    "Manager": ["manager"],
    "Entry": ["entry", "entry level", "entry-level", "junior"],
    "Staff": ["staff", "principal"],
}

def search_salary_job(job_name: str, seniority: str) -> float:
    load_dotenv()
    api_key = os.environ.get("JOBDATALAKE_API_KEY")
    if not api_key:
        raise RuntimeError("JOBDATALAKE_API_KEY not found in the environment")

    api_seniorities = SENIORITY_MAP.get(seniority, [seniority.lower()])

    params = {
        "q": job_name,
        "per_page": "50",
        "salary_min": "100",
    }
    headers = {"X-API-Key": api_key}

    resp = requests.get(
        "https://api.jobdatalake.com/v1/jobs",
        headers=headers,
        params=params,
        timeout=15,
    )
    resp.raise_for_status()
    data = resp.json()

    jobs = data.get("jobs", [])
    if not jobs:
        return 0.0

    salaries = []
    for job in jobs:
        job_sen = [s.lower() for s in job.get("seniority", [])]
        if not any(any(aps in js or js in aps for aps in api_seniorities) for js in job_sen):
            continue

        sal_min = job.get("salary_min_usd")
        sal_max = job.get("salary_max_usd")
        if sal_min is not None or sal_max is not None:
            sal_min = sal_min or 0
            sal_max = sal_max or sal_min
            annual_min = sal_min * 1000
            annual_max = sal_max * 1000
            salaries.append((annual_min + annual_max) / 2)

    if not salaries:
        return 0.0

    return round(sum(salaries) / len(salaries), 2)


def search_salary_with_ai(job_name: str, country: str, seniority: str) -> float:
    load_dotenv()
    api_key = os.environ.get("GEMINI_API_KEY")
    if not api_key:
        raise RuntimeError("GEMINI_API_KEY not found in the environment")

    client = genai.Client(api_key=api_key)

    prompt = f"""
    Search the web for the AVERAGE ANNUAL GROSS SALARY for a
    "{seniority}" "{job_name}" position in {country}.

    Prefer salary surveys, glassdoor, levels.fyi, or similar reliable sources.
    Return ONLY the annual gross salary as a single number in USD,
    with no extra text, no markdown, no explanation.
    If you find a range, return the midpoint.
    If the salary is in a local currency, convert it to USD using current exchange rates.
    """

    response = client.models.generate_content(
        model="gemini-2.5-flash",
        contents=[prompt],
        config=types.GenerateContentConfig(
            tools=[types.Tool(google_search=types.GoogleSearch())],
            temperature=0,
        ),
    )

    text = response.text.strip()

    range_match = re.search(
        r"(\d+(?:,\d{3})*(?:\.\d+)?)\s*(?:-|–|—|to|and)\s*(\d+(?:,\d{3})*(?:\.\d+)?)",
        text,
        re.IGNORECASE,
    )
    if range_match:
        low = float(range_match.group(1).replace(",", ""))
        high = float(range_match.group(2).replace(",", ""))
        return round((low + high) / 2, 2)

    numbers = re.findall(r"\d+(?:,\d{3})*(?:\.\d+)?", text)
    if numbers:
        return float(numbers[0].replace(",", ""))

    raise RuntimeError(
        f"The AI has not returned a valid number for the salary. "
        f"Response: {text}"
    )


def search_fiscal_params(country: str) -> dict:
    load_dotenv()
    API_KEY = os.environ.get("GEMINI_API_KEY")
    if not API_KEY:
        raise RuntimeError("GEMINI_API_KEY not found in the environment")

    client = genai.Client(api_key=API_KEY)

    prompt = f"""
    Search the web for the OFFICIAL, CURRENT personal income tax brackets
    (national/federal level) for a single employee with no dependents in:
    {country}

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
      "country": "{country}",
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
        model="gemini-2.5-flash",
        contents=[prompt],
        config=types.GenerateContentConfig(
            tools=[types.Tool(google_search=types.GoogleSearch())],
            temperature=0,
        ),
    )

    text = response.text.strip()

    clean_text = re.sub(r"^```(?:json)?\s*|\s*```$", "", text.strip())

    try:
        data = json.loads(clean_text)
    except json.JSONDecodeError as e:
        raise RuntimeError(
            f"The AI hasn't returned a valid JSON. Gross response:\n{text}"
        ) from e

    return data


def search_tax_from_brackets(gross_salary: float, params: dict) -> float:
    fx_rate = usd_to_currency(1.0, params.get("currency", "USD"))
    gross_in_local = gross_salary * fx_rate

    deduction = params.get("standard_deduction", 0) or 0
    taxable = max(0.0, gross_in_local - deduction)

    brackets = params["tax_brackets"]

    tax = 0.0
    previous_threshold = 0.0
    for band in brackets:
        superior_threshold = band["upper_bound"]
        rate = band["rate"]

        if superior_threshold is None:
            if taxable > previous_threshold:
                tax += (taxable - previous_threshold) * rate
            break

        if taxable <= previous_threshold:
            break

        bracket_based_share = min(taxable, superior_threshold) - previous_threshold
        tax += bracket_based_share * rate
        previous_threshold = superior_threshold

    gross_credit = params.get("tax_credit", 0)
    try:
        credit = float(gross_credit) if gross_credit is not None else 0.0
        if credit < 0:
            credit = 0.0
    except (TypeError, ValueError):
        credit = 0.0

    neat_tax = max(0.0, tax - credit)

    return round(neat_tax / fx_rate, 2)

