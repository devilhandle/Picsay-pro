# PicSay ARM64 reconstruction workspace

## Implemented in this workspace

- ABI audit of the supplied APK's x86 and armeabi-v7a native libraries.
- Extracted 45-method JNI contract and native subsystem inventory.
- Ghidra batch pseudocode/function export script and reconstruction workflow.
- Independent, host-testable CPU pixel engine in `core/`: invert, grayscale,
  brightness/contrast/saturation, box blur, sharpen, alpha compositing, crop,
  quarter-turn rotation, pixelization, and bulge warp.
- CI builds/tests the CPU core on Linux and cross-compiles it for Android
  `arm64-v8a` using the NDK.

## Important integration status

The CPU engine is a newly written implementation foundation, not a drop-in
replacement for PicSay's proprietary native libraries. It is not yet wired to
the APK's 45 JNI entry points. The original GL warp session and PicSay-specific
filter graph/lifecycle semantics still require reverse engineering and adapter
implementation. Do not package this static library as `libImageOp.so` or
`libGLImageOp.so`: its exported API is different and it is not ABI-compatible.

The CI artifact `picsay-arm64-cpu-engine` is an ARM64 static core library only.
A complete application port requires JNI adapters, image decode/encode integration,
GLES3/EGL implementation, and device-level open-edit-save validation.
