import os, json, sys
from datetime import datetime, timezone

try:
    from curl_cffi import requests
except ImportError:
    import requests

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
RATES_CACHE_PATH = os.path.join(DATA_DIR, "exchangeRates.json")
FX_API_URL = "https://open.er-api.com/v6/latest/USD"
CACHE_TTL_SECONDS = 86400


def _load_cached_rates():
    if not os.path.exists(RATES_CACHE_PATH):
        return None
    try:
        with open(RATES_CACHE_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return None


def _save_cached_rates(rates: dict) -> None:
    try:
        with open(RATES_CACHE_PATH, "w", encoding="utf-8") as f:
            json.dump(rates, f, indent=2, ensure_ascii=False)
    except OSError:
        pass


def get_exchange_rate(target_currency: str) -> float:
    currency = (target_currency or "USD").strip().upper()
    if not currency or currency == "USD":
        return 1.0

    cache = _load_cached_rates()
    rates = {}
    if cache is not None:
        try:
            saved_at = datetime.fromisoformat(cache["saved_at"])
            rates = cache.get("rates", {})
            age = (datetime.now(timezone.utc) - saved_at).total_seconds()
            if age < CACHE_TTL_SECONDS and currency in rates:
                return float(rates[currency])
        except (ValueError, TypeError, KeyError):
            pass

    try:
        resp = requests.get(FX_API_URL, timeout=15)
        resp.raise_for_status()
        data = resp.json()
        rates = data.get("rates", {})
        if currency not in rates:
            print(f"[WARNING]: Currency '{currency}' not found in exchange rates. Assuming rate 1.0.", file=sys.stderr)
            return 1.0
        _save_cached_rates({
            "saved_at": datetime.now(timezone.utc).isoformat(),
            "rates": rates,
        })
        return float(rates[currency])
    except Exception as e:
        if currency in rates:
            return float(rates[currency])
        print(f"[WARNING]: Exchange rate fetch failed for '{currency}': {e}. Assuming rate 1.0.", file=sys.stderr)
        return 1.0


def usd_to_currency(amount_usd: float, target_currency: str) -> float:
    return amount_usd * get_exchange_rate(target_currency)
