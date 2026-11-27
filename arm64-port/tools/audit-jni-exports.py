#!/usr/bin/env python3
"""Extract JNI-relevant native exports and Java loadLibrary references from PicSay APK."""
from __future__ import annotations
import re
import subprocess
import sys
import zipfile

APK = sys.argv[1] if len(sys.argv) > 1 else "Picsay_Pro_v2.3.3.1_1766148552_latestmodapks.com.apk"

def run(*args):
    p = subprocess.run(args, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return p.stdout

def main():
    with zipfile.ZipFile(APK) as apk:
        libs = [n for n in apk.namelist() if n.startswith("lib/") and n.endswith(".so")]
        dexes = [n for n in apk.namelist() if n.endswith(".dex")]
        print("=== Native libraries and exported JNI symbols ===")
        for name in sorted(libs):
            data = apk.read(name)
            tmp = "/tmp/picsay-audit-" + name.rsplit("/", 1)[-1]
            with open(tmp, "wb") as f:
                f.write(data)
            header = run("readelf", "-h", tmp)
            machine = next((x.strip() for x in header.splitlines() if "Machine:" in x), "Machine: unknown")
            symbols = run("readelf", "-Ws", tmp)
            exports = []
            for line in symbols.splitlines():
                if re.search(r"\bGLOBAL\b", line) and re.search(r"\bDEFAULT\b", line) and not re.search(r"\bUND\b", line):
                    fields = line.split()
                    if fields:
                        exports.append(fields[-1])
            jni = sorted(set(x for x in exports if x.startswith("Java_") or x.startswith("JNI_OnLoad")))
            print(f"{name}: {machine}; JNI exports={len(jni)}")
            for symbol in jni:
                print("  " + symbol)
            if not jni:
                print("  (no conventional JNI exports; inspect JNI_OnLoad/dynamic registration)")
        print("=== DEX strings mentioning native loading / JNI ===")
        found = set()
        for dex in dexes:
            data = apk.read(dex)
            strings = re.findall(rb"[\x20-\x7e]{4,}", data)
            for raw in strings:
                value = raw.decode("ascii", "ignore")
                if "loadLibrary" in value or "ImageOp" in value or "GLImageOp" in value or "native" in value.lower():
                    found.add(value[:240])
        for value in sorted(found):
            print("  " + value)
        if not libs or not dexes:
            raise SystemExit("APK missing native libraries or DEX")
        print("Audit complete.")
if __name__ == "__main__":
    main()
