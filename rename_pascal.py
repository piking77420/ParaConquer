#!/usr/bin/env python3
"""
rename_pascal.py
Rename files/folders from snake_case -> PascalCase and update #include lines.

Usage:
  python rename_pascal.py <root_dir> [--dry-run] [--update-guards] [--verbose]

Examples:
  python rename_pascal.py "C:\Data\Isart\Projet\C++\renameTest" --dry-run --verbose
  python rename_pascal.py /path/to/project
"""
from pathlib import Path
import argparse, re, os
from uuid import uuid4

def snake_to_pascal(name: str) -> str:
    return ''.join(part.capitalize() for part in name.split('_') if part != '')

def safe_rename(path: Path, new_name: str, dry_run: bool, verbose: bool) -> Path:
    if path.name == new_name:
        return path
    target = path.with_name(new_name)
    if dry_run:
        if verbose:
            print(f"[DRY] Would rename: {path} -> {target}")
        return target
    try:
        # Windows: if only case differs, do a temp rename first
        if os.name == 'nt' and path.exists() and not target.exists() and path.name.lower() == new_name.lower():
            temp_name = "__tmp__" + uuid4().hex + "__" + path.name
            temp_path = path.with_name(temp_name)
            path.rename(temp_path)
            temp_path.rename(target)
            if verbose:
                print(f"Renamed (case-fix): {path} -> {target}")
        else:
            path.rename(target)
            if verbose:
                print(f"Renamed: {path} -> {target}")
    except Exception as e:
        print(f"ERROR renaming {path} -> {target}: {e}")
    return target

def rename_all(root: Path, dry_run: bool, verbose: bool):
    # collect all paths first, sort deepest-first
    items = [p for p in root.rglob('*')]
    items.sort(key=lambda p: len(p.parts), reverse=True)
    renamed_map = {}  # old_rel_noext -> new_rel_noext (forward slashes)
    for p in items:
        try:
            if not p.exists():
                # parent might have been moved; skip
                continue
            if p.is_file():
                new_name = snake_to_pascal(p.stem) + p.suffix
            else:
                new_name = snake_to_pascal(p.name)
            if new_name != p.name:
                new_path = safe_rename(p, new_name, dry_run, verbose)
                rel_old = str(p.relative_to(root).with_suffix('')).replace('\\','/')
                rel_new = str(new_path.relative_to(root).with_suffix('')).replace('\\','/')
                renamed_map[rel_old] = rel_new
        except Exception as e:
            print(f"ERROR processing {p}: {e}")
    return renamed_map

def rewrite_include_path(inc_path: str) -> str:
    # Preserve leading ./ or ../ parts; preserve separator style (backslash if exclusively used)
    sep = '\\' if ('\\' in inc_path and '/' not in inc_path) else '/'
    parts = re.split(r'[\\/]', inc_path)
    new_parts = []
    for i, part in enumerate(parts):
        if part in ('', '.', '..'):
            new_parts.append(part)
            continue
        # last part may have extension
        if i == len(parts) - 1:
            stem, ext = os.path.splitext(part)
            new_parts.append(snake_to_pascal(stem) + ext)
        else:
            new_parts.append(snake_to_pascal(part))
    return sep.join(new_parts)

def update_includes(root: Path, dry_run: bool, verbose: bool):
    include_re = re.compile(r'(^\s*#\s*include\s*)([<"])([^">]+)([">])', flags=re.MULTILINE)
    files = [p for p in root.rglob('*') if p.suffix.lower() in ('.c', '.cpp', '.cc', '.cxx', '.h', '.hpp', '.hh')]
    for f in files:
        try:
            text = f.read_text(encoding='utf-8')
        except Exception as e:
            print(f"Skipping {f}: cannot read ({e})")
            continue
        changed = False
        def repl(m):
            nonlocal changed
            pre, opener, pathstr, closer = m.groups()
            new_path = rewrite_include_path(pathstr)
            if new_path != pathstr:
                changed = True
                if verbose:
                    print(f"{f}: include {pathstr} -> {new_path}")
            return f"{pre}{opener}{new_path}{closer}"
        new_text = include_re.sub(repl, text)
        if changed:
            if dry_run:
                print(f"[DRY] Would update includes in {f}")
            else:
                try:
                    f.write_text(new_text, encoding='utf-8')
                    print(f"Updated includes in {f}")
                except Exception as e:
                    print(f"ERROR writing {f}: {e}")

def update_include_guards(root: Path, dry_run: bool, verbose: bool):
    # Best-effort: look for first #ifndef / #define pair at top of file and replace guard based on new path
    headers = [p for p in root.rglob('*') if p.suffix.lower() in ('.h', '.hpp', '.hh')]
    for f in headers:
        try:
            full = f.read_text(encoding='utf-8')
        except Exception as e:
            print(f"Skipping guard fix {f}: cannot read ({e})")
            continue
        lines = full.splitlines(True)
        found = False
        for i, L in enumerate(lines[:60]):  # only examine first 60 lines
            m = re.match(r'^\s*#\s*ifndef\s+(\w+)', L)
            if m:
                old_guard = m.group(1)
                # look a few lines ahead for matching define
                for j in range(i+1, min(i+8, len(lines))):
                    m2 = re.match(r'^\s*#\s*define\s+(\w+)', lines[j])
                    if m2 and m2.group(1) == old_guard:
                        # compute new guard from the file's relative path
                        rel = str(f.relative_to(root).with_suffix('')).replace('\\','/').upper()
                        new_guard = re.sub(r'[^A-Z0-9]', '_', rel) + '_'
                        new_full = full.replace(old_guard, new_guard)
                        if dry_run:
                            print(f"[DRY] Would update guard in {f}: {old_guard} -> {new_guard}")
                        else:
                            try:
                                f.write_text(new_full, encoding='utf-8')
                                print(f"Updated include guard in {f}: {old_guard} -> {new_guard}")
                            except Exception as e:
                                print(f"ERROR writing guard update for {f}: {e}")
                        found = True
                        break
                break
        if not found and verbose:
            print(f"No include-guard pair found to update in {f}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('root', type=str, help="project root directory")
    parser.add_argument('--dry-run', action='store_true', help="show changes but don't write")
    parser.add_argument('--update-guards', action='store_true', help="try to update include guards (#ifndef/#define)")
    parser.add_argument('--verbose', action='store_true', help="verbose output")
    args = parser.parse_args()

    root = Path(args.root)
    if not root.is_dir():
        print("Root must be an existing directory.")
        return

    print("STEP 1: Renaming files and directories...")
    renamed_map = rename_all(root, args.dry_run, args.verbose)
    if args.verbose and renamed_map:
        print("Renamed map (old->new):")
        for k,v in renamed_map.items():
            print(f"  {k} -> {v}")

    print("STEP 2: Updating #include lines...")
    update_includes(root, args.dry_run, args.verbose)

    if args.update_guards:
        print("STEP 3: Updating include guards (best-effort)...")
        update_include_guards(root, args.dry_run, args.verbose)

    print("Done.")

if __name__ == '__main__':
    main()
