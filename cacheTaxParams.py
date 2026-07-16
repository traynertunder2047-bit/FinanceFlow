"""
{
  "italy": {
    "parametri": { ... stessa struttura restituita da cerca_parametri_fiscali/estrai_parametri_da_pdf ... },
    "fonte_originale": "pdf" | "web",
    "salvato_il": "2026-07-12T15:30:00"
  },
  "poland": { ... },
  ...
}
"""
import os, json
from datetime import datetime, timezone
from typing import Optional

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CACHE_PATH = os.path.join(SCRIPT_DIR, "fiscalCacheParams.json")


def _carica_cache() -> dict:
    if not os.path.exists(CACHE_PATH):
        return {}
    try:
        with open(CACHE_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        # File corrotto o illeggibile: meglio ripartire da una cache vuota
        # che far crashare tutto il programma.
        return {}


def _salva_cache(cache: dict) -> None:
    with open(CACHE_PATH, "w", encoding="utf-8") as f:
        json.dump(cache, f, indent=2, ensure_ascii=False)


def leggi_da_cache(paese: str) -> Optional[dict]:
    cache = _carica_cache()
    chiave = paese.strip().lower()
    return cache.get(chiave)


def scrivi_in_cache(paese: str, parametri: dict, fonte_originale: str) -> None:
    """Salva (o sovrascrive) l'entry di cache per il paese richiesto."""
    cache = _carica_cache()
    chiave = paese.strip().lower()
    cache[chiave] = {
        "parametri": parametri,
        "fonte_originale": fonte_originale,
        "salvato_il": datetime.now(timezone.utc).isoformat(timespec="seconds"),
    }
    _salva_cache(cache)


def svuota_cache(paese: Optional[str] = None) -> None:
    if paese is None:
        _salva_cache({})
        return

    cache = _carica_cache()
    chiave = paese.strip().lower()
    if chiave in cache:
        del cache[chiave]
        _salva_cache(cache)