#!/usr/bin/env python3
"""Generate Hugo-friendly Markdown API pages from Doxygen XML with Moxygen."""

import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET


def output_modes(index: Path) -> list[str]:
    """Enable only Moxygen output modes actually present in Doxygen XML."""
    try:
        root = ET.parse(index).getroot()
    except ET.ParseError as exc:
        raise ValueError(f"Invalid Doxygen XML index {index}: {exc}") from exc

    kinds = {compound.get("kind") for compound in root.findall("compound")}
    if not kinds:
        raise ValueError(f"No Doxygen compounds found in {index}")

    modes = []
    # Doxygen uses separate kinds for classes, structs, unions and interfaces.
    if kinds.intersection({"class", "struct", "union", "interface"}):
        modes.append("--classes")
    if "group" in kinds:
        modes.append("--groups")
    if "page" in kinds:
        modes.append("--pages")
    return modes


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xml", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    index = args.xml / "index.xml"
    if not index.is_file():
        parser.error(f"Doxygen XML index not found: {index}")
    tool = shutil.which("moxygen")
    if tool is None:
        parser.error("Install Node.js 20+ and run npm install -g moxygen@2.1.19")

    try:
        modes = output_modes(index)
    except ValueError as exc:
        parser.error(str(exc))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="utility-docs-", dir=args.output.parent) as tmp:
        stage = Path(tmp) / "markdown"
        stage.mkdir()
        # When no compound category is split into individual pages, use the
        # default single-page Moxygen output instead of a %s filename.
        filename = "api-%s.md" if modes else "api.md"
        command = [
            tool, *modes, "--anchors", "--frontmatter", "--flavour", "github",
            "--output", str(stage / filename), str(args.xml)
        ]
        print("Moxygen output modes:", " ".join(modes) if modes else "single page", flush=True)
        subprocess.run(command, check=True)

        pages = sorted(stage.rglob("*.md"))
        if not pages:
            print("Moxygen produced no Markdown pages", file=sys.stderr)
            return 1
        # Replace the output only when generation succeeded. This also removes
        # obsolete pages that could otherwise survive into a Hugo publication.
        if args.output.exists():
            shutil.rmtree(args.output)
        shutil.move(str(stage), str(args.output))
        print(f"Generated {len(pages)} Markdown pages in {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
