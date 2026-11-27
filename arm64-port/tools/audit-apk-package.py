#!/usr/bin/env python3
"""Audit the checked-in PicSay APK's native libraries and package layout."""
from __future__ import annotations
import struct
import sys
import zipfile
from pathlib import PurePosixPath

APK = sys.argv[1] if len(sys.argv) > 1 else "Picsay_Pro_v2.3.3.1_1766148552_latestmodapks.com.apk"
ABI_MACHINES = {3: "armeabi/x86", 40: "armeabi-v7a/ARM", 62: "x86_64", 183: "arm64-v8a/AArch64"}

def elf_info(data: bytes):
    if len(data) < 20 or data[:4] != b"\x7fELF":
        return "not-ELF"
    elf_class = {1: "ELF32", 2: "ELF64"}.get(data[4], f"class-{data[4]}")
    endian = "<" if data[5] == 1 else ">" if data[5] == 2 else None
    if endian is None:
        return f"{elf_class}, unknown-endian"
    machine = struct.unpack(endian + "H", data[18:20])[0]
    return f"{elf_class}, {ABI_MACHINES.get(machine, 'machine-' + str(machine))}"

def main():
    try:
        apk = zipfile.ZipFile(APK)
    except (OSError, zipfile.BadZipFile) as exc:
        raise SystemExit(f"Cannot open APK: {exc}")
    names = apk.namelist()
    dex = sorted(n for n in names if PurePosixPath(n).name.startswith("classes") and n.endswith(".dex"))
    libs = sorted(n for n in names if n.startswith("lib/") and n.endswith(".so"))
    print(f"APK: {APK}")
    print(f"DEX files: {len(dex)}")
    print(f"Native libraries: {len(libs)}")
    abi_dirs = sorted({n.split("/")[1] for n in libs if len(n.split("/")) > 2})
    print("ABI directories: " + (", ".join(abi_dirs) if abi_dirs else "NONE"))
    if not dex:
        raise SystemExit("FAIL: no DEX bytecode found")
    if not libs:
        raise SystemExit("FAIL: no native libraries found")
    for name in libs:
        print(f"{name}: {elf_info(apk.read(name))}")
    print("DEX inventory:")
    for name in dex:
        print(f"  {name}: {len(apk.read(name))} bytes")
    print("Audit completed.")

if __name__ == "__main__":
    main()
