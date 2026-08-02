import os, sys, json

from rich.console import Console

from app.pdfChapterExtractor import extract_chapter_text
from app.pdfParamExtractor import extract_params_from_pdf
from app.scraper import search_fiscal_params, search_tax_from_brackets
from app.cacheTaxParams import read_from_cache, write_in_cache, empty_cache

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
PDF_MAP_PATH = os.path.join(DATA_DIR, "countryChapters.json")
PDF_PATH = os.path.join(DATA_DIR, "TaxingWages2026.pdf")


def _log(message: str, style: str | None = None) -> None:
    if style is None:
        print(message, file=sys.stderr)
    else:
        Console(file=sys.stderr).print(message, style=style)


def country_present_in_pdf(country: str) -> bool:
    if not os.path.exists(PDF_MAP_PATH):
        return False
    with open(PDF_MAP_PATH, "r", encoding="utf-8") as f:
        _map = json.load(f)

    key = country.strip().lower()
    if key in _map:
        return True
    candidates = [k for k in _map if key in k or k in key]
    return len(candidates) == 1


def calculate_tax(country: str, gross_annual_salary: float, forced_refresh: bool = False) -> dict:
    """
      - "tax": float, calculated tax
      - "source": str, what strategy was used
                 ("cache_pdf" | "cache_web" | "pdf" | "web")
      - "params": dict, the brackets/params used for calculation
    """
    if forced_refresh:
        _log(f"Forced refresh requested for '{country}'.")
        entry_cache = read_from_cache(country)
        if entry_cache is not None:
            _log(f"Cache entry for '{country}' deleted.")
        else:
            _log(f"'{country}' wasn't in the cache - nothing to refresh.")
        empty_cache(country)
    else:
        entry_cache = read_from_cache(country)  
        if entry_cache is not None:
            _log(
                f"'{country}' found in cache (saved the {entry_cache['saved_the']}, "
                f"original_source: {entry_cache['original_source']}). AI calls: none.",
                style="dim",
            )
            return {
                "tax": search_tax_from_brackets(gross_annual_salary, entry_cache["params"]),
                "source": f"cache_{entry_cache['original_source']}",
                "params": entry_cache["params"],
            }

    
    if country_present_in_pdf(country) and os.path.exists(PDF_PATH):
        _log(f"'{country}' not in cache. Found in the PDF's index: extracting the brackets of the dedicated chapters.")
        chapter_text = extract_chapter_text(country, PDF_PATH)
        params = extract_params_from_pdf(country, chapter_text)
        source = "pdf"
    else:
        _log(f"'{country}' not in cache and not in the PDF's index. Searching the brackets on the web...")
        params = search_fiscal_params(country)
        source = "web"

    write_in_cache(country, params, source)
    _log(f"Params for '{country}' saved in cache for the next executions.")

    tax = search_tax_from_brackets(gross_annual_salary, params)
    
    return {
        "tax": tax,
        "source": source,
        "params": params,
    }
