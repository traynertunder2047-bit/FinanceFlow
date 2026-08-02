"""
{
  "italy": {
    "params": { ... same structure received from search_fiscal_params/extract_params_from_pdf ... },
    "original_source": "pdf" | "web",
    "saved_the": "2026-07-12T15:30:00"
  },
  "poland": { ... },
  ...
}
"""
import os, json
from datetime import datetime, timezone
from typing import Optional

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
CACHE_PATH = os.path.join(DATA_DIR, "fiscalCacheParams.json")


def load_cache() -> dict:
    if not os.path.exists(CACHE_PATH):
        return {}
    try:
        with open(CACHE_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return {}


def save_cache(cache: dict) -> None:
    with open(CACHE_PATH, "w", encoding="utf-8") as f:
        json.dump(cache, f, indent=2, ensure_ascii=False)


def read_from_cache(country: str) -> Optional[dict]:
    cache = load_cache()
    key = country.strip().lower()
    return cache.get(key)


def write_in_cache(country: str, params: dict, original_source: str) -> None:
    """Save (or overwrites) the entry of the cache for the requested country."""
    cache = load_cache()
    key = country.strip().lower()
    cache[key] = {
        "params": params,
        "original_source": original_source,
        "saved_the": datetime.now(timezone.utc).isoformat(timespec="seconds"),
    }
    save_cache(cache)


def empty_cache(country: Optional[str] = None) -> None:
    if country is None:
        save_cache({})
        return

    cache = load_cache()
    key = country.strip().lower()
    if key in cache:
        del cache[key]
        save_cache(cache)