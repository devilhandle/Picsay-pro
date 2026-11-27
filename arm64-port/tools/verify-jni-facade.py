#!/usr/bin/env python3
"""Verify Java native declarations match the JNI RegisterNatives table."""
import pathlib
import re
import sys

root = pathlib.Path(__file__).resolve().parents[1] / "core"
java = (root / "java/com/devilhandle/picsay/NativeImageEngine.java").read_text()
cpp = (root / "src/AndroidBitmapBridge.cpp").read_text()

types = {
    "Bitmap": "Landroid/graphics/Bitmap;",
    "float": "F",
    "int": "I",
    "boolean": "Z",
    "float[]": "[F",
}
declared = {}
for name, args in re.findall(r"public\s+static\s+native\s+boolean\s+(\w+)\(([^)]*)\)\s*;", java):
    params = []
    for arg in filter(None, (part.strip() for part in args.split(","))):
        pieces = arg.split()
        if len(pieces) != 2 or pieces[0] not in types:
            raise SystemExit(f"Unsupported Java native parameter: {arg}")
        params.append(types[pieces[0]])
    declared[name] = "(" + "".join(params) + ")Z"

registered = {}
for name, signature in re.findall(
    r'const_cast<char\*>\("([A-Za-z0-9_]+)"\),\s*const_cast<char\*>\("([^"]+)"\)',
    cpp,
):
    registered[name] = signature

errors = []
for name, signature in declared.items():
    actual = registered.get(name)
    if actual != signature:
        errors.append(f"{name}: Java={signature}, JNI={actual}")
for name in registered.keys() - declared.keys():
    errors.append(f"{name}: registered in JNI but missing from Java facade")
if errors:
    print("\n".join(errors), file=sys.stderr)
    raise SystemExit(1)
print(f"JNI facade verified: {len(declared)} native methods match exactly.")
