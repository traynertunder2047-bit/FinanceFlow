import os, json
from pypdf import PdfReader

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
MAP_PATH = os.path.join(DATA_DIR, "countryChapters.json")


def create_chapter_map(pdf_path: str) -> dict:
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
            "Didn't find the section 'Part II Country details' in the online PDF. "
            "The document's structure could be changed: the map has to be reconstructed by hand."
        )

    country_map = {}
    for idx, bm in enumerate(country_bookmarks):
        display_name = bm.title.strip().split(" (")[0].strip()
        start_page = reader.get_destination_page_number(bm) + 1  # 1-indexed

        if idx + 1 < len(country_bookmarks):
            next_start = reader.get_destination_page_number(country_bookmarks[idx + 1]) + 1
            end_page = next_start - 1
        else:
            end_page = start_page + 20 

        country_map[display_name.lower()] = {
            "display_name": display_name,
            "start_page": start_page,
            "end_page": end_page,
        }

    return country_map


def load_or_create_map(pdf_path: str) -> dict:
    if os.path.exists(MAP_PATH):
        with open(MAP_PATH, "r", encoding="utf-8") as f:
            return json.load(f)

    _map = create_chapter_map(pdf_path)
    with open(MAP_PATH, "w", encoding="utf-8") as f:
        json.dump(_map, f, indent=2, ensure_ascii=False)
    return _map


def extract_chapter_text(country_name: str, pdf_path: str) -> str:
    _map = load_or_create_map(pdf_path)

    key = country_name.strip().lower()
    if key not in _map:
        candidates = [k for k in _map if key in k or k in key]
        if len(candidates) == 1:
            key = candidates[0]
        else:
            available = ", ".join(sorted(v["display_name"] for v in _map.values()))
            raise ValueError(
                f"Country '{country_name}' not found in the chapters map. "
                f"Available countries: {available}"
            )

    info = _map[key]
    reader = PdfReader(pdf_path)

    text = f"[Chapter: {info['display_name']} - Taxing Wages 2026]\n\n"
    for i in range(info["start_page"] - 1, info["end_page"]):
        page_text = reader.pages[i].extract_text()
        if page_text:
            text += page_text + "\n"

    return text
