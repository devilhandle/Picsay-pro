# PicSay ARM64 reconstruction workspace

## Implemented source

- ABI audit of the supplied APK's x86 and armeabi-v7a native libraries.
- Extracted 45-method legacy JNI contract and native subsystem inventory.
- Ghidra function/pseudocode export script and reconstruction workflow.
- Independent C++17 CPU image engine: invert, grayscale, brightness/contrast/saturation,
  box blur, sharpen, alpha compositing, crop, quarter-turn rotation, pixelize, bulge.
- Additional effects: temperature, sepia, 4x5 color matrix, Sobel, vignette, twirl,
  spherize, linear stretch, circle-splash distortion, and color mixing.
- Android Bitmap JNI facade in `java/com/devilhandle/picsay/NativeImageEngine.java`
  and `src/AndroidBitmapBridge.cpp`. It handles mutable RGBA_8888 bitmaps with
  Android row-stride and lock/unlock semantics.
- CMake builds `libpicsay_engine.so` for Android and host-side test executables.
- GitHub Actions runs host tests and cross-compiles the Android arm64-v8a JNI library.

## Build locally

Host:
```bash
cmake -S arm64-port/core -B build/host -DCMAKE_BUILD_TYPE=Release
cmake --build build/host --parallel
ctest --test-dir build/host --output-on-failure
```

Android ARM64 (Android NDK required):
```bash
cmake -S arm64-port/core -B build/android-arm64 \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
  -DPICSAY_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build/android-arm64 --parallel
```

The shared library is `libpicsay_engine.so`; the static core is
`libPicSayImageEngine.a`. CI publishes both as a workflow artifact.

## Compatibility boundary — do not misrepresent this build

This is a new engine and a new JNI facade. It is **not** a binary-compatible
replacement for the proprietary PicSay `libImageOp.so` or `libGLImageOp.so`.
The Java facade currently exposes the new package
`com.devilhandle.picsay.NativeImageEngine`, not the original application's 45
native declarations. The legacy classes, native pointer handles, tiled filter
graph, codecs/file renderer, brush/stroke serialization, and GLES3/EGL warp
session still need exact reverse engineering and adapter implementation.

Only mutable RGBA_8888 Bitmap inputs are currently accepted by the new bridge.
No complete PicSay APK has been rebuilt or device-tested yet. Do not rename this
library to the legacy filenames or package it as a finished PicSay ARM64 port.
