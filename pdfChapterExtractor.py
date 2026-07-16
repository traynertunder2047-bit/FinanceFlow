import os, json
from pypdf import PdfReader

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MAPPA_PATH = os.path.join(SCRIPT_DIR, "countryChapters.json")


def _costruisci_mappa_capitoli(pdf_path: str) -> dict:
    reader = PdfReader(pdf_path)
    outline = reader.outline

    country_bookmarks = []
    in_part_ii = False
    i = 0
    while i < len(outline):
        item = outline[i]
        if isinstance(item, list):
            if in_part_ii:
                for sub in item:
                    if not isinstance(sub, list):
                        country_bookmarks.append(sub)
                in_part_ii = False
            i += 1
            continue
        if "Part II" in item.title and "Country details" in item.title:
            in_part_ii = True
        i += 1

    if not country_bookmarks:
        raise RuntimeError(
            "Non ho trovato la sezione 'Part II Country details' nell'outline del PDF. "
            "La struttura del documento potrebbe essere cambiata: la mappa va ricostruita a mano."
        )

    country_map = {}
    for idx, bm in enumerate(country_bookmarks):
        clean_name = bm.title.strip().split(" (")[0].strip()
        start_page = reader.get_destination_page_number(bm) + 1  # 1-indexed

        if idx + 1 < len(country_bookmarks):
            next_start = reader.get_destination_page_number(country_bookmarks[idx + 1]) + 1
            end_page = next_start - 1
        else:
            end_page = start_page + 20  # fallback per l'ultimo paese della lista

        country_map[clean_name.lower()] = {
            "display_name": clean_name,
            "start_page": start_page,
            "end_page": end_page,
        }

    return country_map


def _carica_o_crea_mappa(pdf_path: str) -> dict:
    if os.path.exists(MAPPA_PATH):
        with open(MAPPA_PATH, "r", encoding="utf-8") as f:
            return json.load(f)

    mappa = _costruisci_mappa_capitoli(pdf_path)
    with open(MAPPA_PATH, "w", encoding="utf-8") as f:
        json.dump(mappa, f, indent=2, ensure_ascii=False)
    return mappa


def estrai_testo_capitolo(nome_paese: str, pdf_path: str) -> str:
    mappa = _carica_o_crea_mappa(pdf_path)

    chiave = nome_paese.strip().lower()
    if chiave not in mappa:
        candidati = [k for k in mappa if chiave in k or k in chiave]
        if len(candidati) == 1:
            chiave = candidati[0]
        else:
            disponibili = ", ".join(sorted(v["display_name"] for v in mappa.values()))
            raise ValueError(
                f"Paese '{nome_paese}' non trovato nella mappa capitoli. "
                f"Paesi disponibili: {disponibili}"
            )

    info = mappa[chiave]
    reader = PdfReader(pdf_path)

    testo = f"[Capitolo: {info['display_name']} - Taxing Wages 2026]\n\n"
    for i in range(info["start_page"] - 1, info["end_page"]):
        pagina_testo = reader.pages[i].extract_text()
        if pagina_testo:
            testo += pagina_testo + "\n"

    return testo


if __name__ == "__main__":
    import sys

    paese = sys.argv[1] if len(sys.argv) > 1 else "Italy"
    pdf = sys.argv[2] if len(sys.argv) > 2 else "TaxingWages2026.pdf"

    testo = estrai_testo_capitolo(paese, pdf)
    print(f"Estratti {len(testo)} caratteri (~{len(testo)//4} token stimati) per '{paese}'")
    print("---")
    print(testo[:400])