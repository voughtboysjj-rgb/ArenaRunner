#!/usr/bin/env python3
"""Inline three.js into index.template.html -> ../docs/index.html

The output is a single self-contained file: no CDN, no network, works from
file:// and from GitHub Pages.

Usage:  python3 Web/build_web.py
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent
TEMPLATE = ROOT / "index.template.html"
VENDOR = ROOT / "vendor" / "three.min.js"
OUTPUT = ROOT.parent / "docs" / "index.html"
MARKER = "/*__THREE_LIB__*/"


def main() -> int:
    template = TEMPLATE.read_text(encoding="utf-8")
    if MARKER not in template:
        print(f"error: {MARKER} not found in {TEMPLATE}", file=sys.stderr)
        return 1

    if not VENDOR.exists():
        print(
            f"error: {VENDOR} is missing.\n"
            "Download it with:\n"
            "  curl -L -o Web/vendor/three.min.js "
            "https://cdnjs.cloudflare.com/ajax/libs/three.js/0.160.0/three.min.js",
            file=sys.stderr,
        )
        return 1

    lib = VENDOR.read_text(encoding="utf-8").replace("</script", "<\\/script")
    output = template.replace(MARKER, lib)

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(output, encoding="utf-8")

    game = re.findall(r"<script>(.*?)</script>", output, flags=re.S)[-1]
    print(f"wrote {OUTPUT} ({len(output):,} bytes, game script {len(game):,} chars)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
