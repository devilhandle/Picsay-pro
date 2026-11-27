# ARM64 validation plan

Do not treat a successful APK install or launch as proof that the image engine works. The Java UI can launch while native image operations fail later.

## 1. Device ABI confirmation

On the target Android device, read the actual ABI list (do not infer it only from CPU marketing names):

```sh
adb shell getprop ro.product.cpu.abilist
adb shell getprop ro.product.cpu.abilist32
adb shell getprop ro.product.cpu.abilist64
```

The target must advertise `arm64-v8a` for the ARM64 package.

## 2. Static package checks

Run:

```sh
python3 arm64-port/tools/extract_jni_contract.py path/to/PicSay.apk
bash arm64-port/tools/verify-arm64.sh path/to/PicSay.apk
```

Expected:
- 45 JNI declarations are reported.
- Both `lib/arm64-v8a/libImageOp.so` and `lib/arm64-v8a/libGLImageOp.so` exist.
- Both files report ELF64 and AArch64.
- `libGLImageOp.so` declares `libImageOp.so` as a dependency.
- No x86 ELF object is packaged under an ARM ABI directory.

## 3. Runtime JNI checks

Install and capture logs:

```sh
adb install --abi arm64-v8a path/to/PicSay.apk
adb logcat -c
# Launch PicSay, open an image, invoke editing, then export.
adb logcat -d | grep -Ei 'UnsatisfiedLinkError|dlopen failed|JNI DETECTED ERROR|ImageOp|GLImageOp|linker'
```

Pass criteria:
- Both native libraries load.
- All native registration calls succeed.
- No unresolved symbol, wrong ELF class, or JNI signature mismatch occurs.
- Native handles are created and destroyed without leaks or double frees.

## 4. Functional image test matrix

| Test | Required assertion |
|---|---|
| Open JPEG | Decode completes; dimensions and orientation are correct |
| Open PNG with alpha | Alpha channel remains correct |
| Bitmap import/export | No stride, pixel-format, or premultiplication corruption |
| Basic color effect | Preview and final render match expected output |
| Crop / rotate / resize | Correct bounds and no memory overrun |
| Blend / mask / red-eye | Pixel output is valid and stable |
| Brush stroke | Points, brush caps/joins, undo and redo remain consistent |
| Warp brush | GLES3/EGL path initializes, renders, and releases resources |
| Save JPEG / PNG | File opens in an independent viewer; dimensions and alpha rules are correct |
| Cancel long operation | Worker stops safely and releases decoder/filter state |
| Large image | No crash, runaway memory use, or tile-boundary artifacts |

## 5. Release gate

A build is not complete until all static checks and the functional matrix pass on a real AArch64 Android device. A dummy JNI implementation, renamed ARMv7/x86 binary, or UI-only launch does not satisfy this gate.
