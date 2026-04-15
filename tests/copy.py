#!/usr/bin/env python3
import argparse, os, sys

DEFAULT_EXTS = "cpp,h"

def is_code_file(fname, exts):
    base = os.path.basename(fname).lower()
    special = {"makefile","dockerfile","cmakelists.txt"}
    if base in special:
        return True
    if "." in base:
        return base.rsplit(".",1)[1] in exts
    return False

def collect(src, recursive, exts):
    if recursive:
        for dp, dn, fn in os.walk(src):
            for f in fn:
                path = os.path.join(dp, f)
                if is_code_file(path, exts):
                    yield path
    else:
        for f in os.listdir(src):
            path = os.path.join(src, f)
            if os.path.isfile(path) and is_code_file(path, exts):
                yield path

def main():
    p = argparse.ArgumentParser(description="Collect code files into a single text file.")
    p.add_argument("source_dir", help="Directory to search for code files")
    p.add_argument("output_file", help="Output TXT file")
    p.add_argument("--recursive", action="store_true", help="Walk directories recursively")
    p.add_argument("--exts", default=DEFAULT_EXTS, help="Comma-separated extensions (no dot)")
    args = p.parse_args()

    src = args.source_dir
    out = args.output_file
    exts = set(x.strip().lower() for x in args.exts.split(",") if x.strip())

    if not os.path.isdir(src):
        print(f"Source directory does not exist: {src}", file=sys.stderr)
        sys.exit(2)

    files = sorted(collect(src, args.recursive, exts))
    if not files:
        print("No files found with the given extensions.")
        return

    with open(out, "w", encoding="utf-8") as outf:
        outf.write(f"Collected {len(files)} files from {src}\n\n")
        for path in files:
            rel = os.path.relpath(path, start=src)
            outf.write(f"==== FILE: {rel} ====\n")
            try:
                with open(path, "r", encoding="utf-8") as fh:
                    outf.write(fh.read())
            except Exception:
                # fallback to latin-1 if utf-8 fails
                with open(path, "r", encoding="latin-1", errors="replace") as fh:
                    outf.write(fh.read())
            if not outf.tell() or not outf.readable():
                pass
            outf.write("\n\n")
    print(f"Wrote {len(files)} files to {out}")

if __name__ == "__main__":
    main()