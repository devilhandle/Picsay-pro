# Reference-binary reverse-engineering workspace

Run `bash arm64-port/tools/audit-reference-native.sh path/to/PicSay.apk` on a Linux
host with `unzip`, `readelf`, and `nm` (or their LLVM equivalents) installed.

The script extracts the original x86 and ARMv7 libraries without modifying them and
records ELF headers, dynamic dependencies, and exported symbols. These are the first
inputs for a Ghidra-based comparison of the two reference implementations.

## Reverse-engineering sequence

1. Compare x86 and ARMv7 exports and JNI registration functions.
2. Load both ARMv7 libraries into the same Ghidra project with the Android ARM
   language/compiler specification; identify JNI_OnLoad registration tables.
3. Recover structures, ownership/lifetime rules, pixel formats, and call graphs.
4. Implement and test the CPU image pipeline for AArch64.
5. Reconstruct the GLES3/EGL warp path separately.
6. Validate on a real arm64 device using the functional matrix in
   [ARM64-VALIDATION-PLAN.md](ARM64-VALIDATION-PLAN.md).

The APK has no native source. The audit script intentionally does not generate fake
JNI stubs or relabel 32-bit objects. A successful ARM64 build requires a genuine
implementation of both native libraries.
