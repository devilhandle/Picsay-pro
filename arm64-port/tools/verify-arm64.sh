#!/usr/bin/env bash
set -euo pipefail

APK_OR_DIR="${1:?Usage: verify-arm64.sh <apk-or-unpacked-directory>}"
TMP=""
if [[ -f "$APK_OR_DIR" ]]; then
  TMP="$(mktemp -d)"
  trap 'rm -rf "$TMP"' EXIT
  unzip -q "$APK_OR_DIR" 'lib/*/*.so' -d "$TMP"
  ROOT="$TMP"
else
  ROOT="$APK_OR_DIR"
fi

fail=0
for lib in libImageOp.so libGLImageOp.so; do
  file="$ROOT/lib/arm64-v8a/$lib"
  if [[ ! -f "$file" ]]; then
    echo "FAIL: missing lib/arm64-v8a/$lib"
    fail=1
    continue
  fi
  header="$(readelf -h "$file")"
  grep -q 'Class:.*ELF64' <<<"$header" || { echo "FAIL: $lib is not ELF64"; fail=1; }
  grep -q 'Machine:.*AArch64' <<<"$header" || { echo "FAIL: $lib is not AArch64"; fail=1; }
  echo "OK: $file"
done

if [[ -f "$ROOT/lib/arm64-v8a/libGLImageOp.so" ]]; then
  readelf -d "$ROOT/lib/arm64-v8a/libGLImageOp.so" | grep -F 'Shared library: [libImageOp.so]' >/dev/null || {
    echo "FAIL: libGLImageOp.so does not declare libImageOp.so dependency"
    fail=1
  }
fi

if [[ "$fail" -ne 0 ]]; then
  echo "ARM64 verification failed."
  exit 1
fi
echo "ELF ABI and direct dependency checks passed. Runtime JNI/image-edit tests are still required."
