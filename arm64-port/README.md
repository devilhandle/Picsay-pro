# PicSay Pro ARM64 native port foundation

## Current status

This branch contains an ABI audit and a build-system foundation only. It does **not** contain working ARM64 native libraries yet.

The uploaded APK contains these native libraries:

| Library | x86 | armeabi-v7a | arm64-v8a |
|---|---:|---:|---:|
| libImageOp.so | ELF32 / Intel 80386 | ELF32 / ARM EABI5 | Missing |
| libGLImageOp.so | ELF32 / Intel 80386 | ELF32 / ARM EABI5 | Missing |

Renaming `lib/x86/` to `lib/arm64-v8a/` cannot change the machine code. Both x86 files are 32-bit Intel ELF objects, not AArch64.

## What the audit found

- `libImageOp.so` exports the native image-processing engine and JNI registration entry points for:
  - `com/Shinycore/Shared/TimImageProxy`
  - `com/Shinycore/PicSay/Tasks/SCImageConverter`
  - `com/Shinycore/Shared/Tasks/SCImageTask`
  - `com/Shinycore/PicSay/Tasks/SCImageFilter`
  - `com/Shinycore/PicSay/Tasks/PSDocumentRendererToFile`
  - `com/Shinycore/PicSayUI/Filters/PEStroke`
- `libGLImageOp.so` registers `com/Shinycore/PicSayUI/Filters/PEWarpSession` and links against `libImageOp.so`, `libGLESv3.so`, and `libEGL.so`.
- The x86 and armeabi-v7a builds expose the same 343 application-level symbols in `libImageOp.so`; the extra ARM exports are ARM EABI/unwind helpers. The two `libGLImageOp.so` builds likewise share their application-level exports.
- The APK does not include native source code or an arm64 build. The shipped armeabi-v7a binary is useful as a behavioral/reference implementation, but it cannot be relabeled or directly linked into an arm64 process.

## Required next step for a real ARM64 port

Rebuild both libraries from their original C/C++ sources with the Android NDK for `arm64-v8a`, preserving JNI registration signatures and the dependency relationship:

1. Build `libImageOp.so` for AArch64.
2. Build `libGLImageOp.so` for AArch64 and link it to the new `libImageOp.so`.
3. Run JNI registration and image-editing tests on an arm64 device.
4. Package the output under `lib/arm64-v8a/` and verify each file with `readelf -h` (ELF64, AArch64).

If the original source is unavailable, a genuine port requires carefully reconstructing the native implementation from the reference binaries and JNI contracts. A dummy library or renamed 32-bit binary would only hide the missing functionality and is intentionally not supplied here.

## Reverse-engineering artifacts

- `JNI-CONTRACT.md`: all 45 native declarations extracted from `classes.dex`, with exact JNI descriptors.
- `NATIVE-ENGINE-INVENTORY.md`: ARMv7 native export inventory and identified image-processing subsystems.
- `tools/extract_jni_contract.py`: standalone DEX parser to reproduce the native-method list.
- `tools/verify-arm64.sh`: checks that both packaged libraries are ELF64/AArch64 and that the GL library links to `libImageOp.so`.

## Build scaffold

See `CMakeLists.txt`. It deliberately refuses to build until the real native source files are added; this prevents accidentally producing nonfunctional placeholder libraries.
