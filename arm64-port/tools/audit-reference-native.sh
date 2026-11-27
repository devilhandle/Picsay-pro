#!/usr/bin/env bash
set -euo pipefail

APK="${1:?Usage: audit-reference-native.sh <PicSay.apk> [output-dir]}"
OUT="${2:-reference-native-audit}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p "$OUT"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

command -v unzip >/dev/null || { echo "unzip is required" >&2; exit 2; }
command -v readelf >/dev/null || { echo "readelf (or llvm-readelf) is required" >&2; exit 2; }
command -v nm >/dev/null || { echo "nm (or llvm-nm) is required" >&2; exit 2; }

for abi in x86 armeabi-v7a arm64-v8a; do
  for lib in libImageOp.so libGLImageOp.so; do
    entry="lib/$abi/$lib"
    if unzip -Z1 "$APK" | grep -Fxq "$entry"; then
      mkdir -p "$OUT/$abi"
      unzip -p "$APK" "$entry" > "$TMP/$lib"
      cp "$TMP/$lib" "$OUT/$abi/$lib"
      readelf -h "$TMP/$lib" > "$OUT/$abi/$lib.elf-header.txt"
      readelf -d "$TMP/$lib" > "$OUT/$abi/$lib.dynamic.txt"
      nm -D --defined-only "$TMP/$lib" 2>/dev/null | awk '{print $NF}' | sort -u > "$OUT/$abi/$lib.exports.txt"
      echo "Extracted $entry"
    else
      echo "Missing $entry"
    fi
  done
done

for lib in libImageOp.so libGLImageOp.so; do
  files=()
  for abi in x86 armeabi-v7a arm64-v8a; do
    f="$OUT/$abi/$lib.exports.txt"
    [[ -f "$f" ]] && files+=("$f")
  done
  if (("${#files[@]}" >= 2)); then
    comm -12 "${files[0]}" "${files[1]}" > "$OUT/$lib.common-exports.txt"
    echo "Common exports for $lib: $(wc -l < "$OUT/$lib.common-exports.txt")"
  fi
done

cat > "$OUT/REPORT.txt" <<REPORT
PicSay native reference extraction
APK: $APK
Generated: $(date -u +"%Y-%m-%dT%H:%M:%SZ")

Each ABI folder contains the original ELF files, ELF headers, dynamic dependencies,
and sorted exported-symbol lists. This is reverse-engineering evidence only; it is
not C/C++ source and cannot be linked as an ARM64 implementation.
REPORT

echo "Audit written to $OUT"
