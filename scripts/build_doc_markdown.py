#!/usr/bin/env python3
"""Generate Hugo-friendly Markdown API pages from Doxygen XML with Moxygen."""

import argparse
from pathlib import Path
import shutil
import subprocess
import sys

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xml", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not (args.xml / "index.xml").is_file():
        parser.error(f"Doxygen XML index not found in {args.xml}")
    tool = shutil.which("moxygen")
    if tool is None:
        parser.error("Install Node.js 20+ and run npm install -g moxygen@2.1.19")
    if args.output.exists():
        shutil.rmtree(args.output)
    args.output.mkdir(parents=True)
    subprocess.run([
        tool, "--groups", "--classes", "--pages", "--anchors",
        "--frontmatter", "--flavour", "github",
        "--output", str(args.output / "api-%s.md"), str(args.xml)
    ], check=True)
    pages = sorted(args.output.rglob("*.md"))
    if not pages:
        print("Moxygen produced no Markdown pages", file=sys.stderr)
        return 1
    print(f"Generated {len(pages)} Markdown pages in {args.output}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
