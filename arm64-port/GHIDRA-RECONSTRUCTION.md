# Ghidra native reconstruction workflow

This is the next executable reverse-engineering step after the ABI/JNI inventory.
It exports function boundaries, signatures, and Ghidra decompiler output from each
reference ELF. It does not pretend pseudocode is original source or directly
buildable C++.

## Prepare inputs

Run the reference extractor:

```bash
bash arm64-port/tools/audit-reference-native.sh Picsay_Pro_v2.3.3.1_1766148552_latestmodapks.com.apk
```

Import these two files into separate Ghidra projects (use the Android ARM little-endian
language/compiler specification for armeabi-v7a):

- `reference-native-audit/armeabi-v7a/libImageOp.so`
- `reference-native-audit/armeabi-v7a/libGLImageOp.so`

Import the x86 counterparts too. Analyze all binaries with function identification,
demangling, references, and ELF relocation processing enabled.

## Export pseudocode

Copy `ghidra_export.py` into Ghidra's user script directory, then run it against
each analyzed program using Script Manager. Set `PICSAY_DECOMP_OUT` to an output
directory before launching Ghidra if desired. The script writes:

- `*.functions.txt`: entry address, symbol name, and inferred signature.
- `*.decompiled.c`: decompiler output for all discovered functions.

## Required manual reconstruction

Prioritize, in order:

1. `JNI_OnLoad` and all `register_*` functions to recover exact native method
   registration and static/instance conventions.
2. `TimImageProxy` constructors/destructors and bitmap/file transfer routines.
3. Decoder/filter graph allocation, ownership, cancellation, and tiled processing.
4. PNG/JPEG output and brush rendering.
5. `PEWarpSession` GLES3/EGL state and resource lifecycle.

Compare x86 and ARMv7 pseudocode for the same symbols to distinguish compiler noise
from algorithmic behavior. Recover structures and constants from cross-references;
do not copy inferred pseudocode blindly into a new implementation.

## Build gate

Only add reconstructed implementation files to the ARM64 CMake target after the
JNI contract and object lifetime rules are understood. Keep the release gate in
`ARM64-VALIDATION-PLAN.md`: ELF64/AArch64 checks plus real open-edit-save and warp
tests on a 64-bit Android device.
