# ABI audit — uploaded PicSay Pro APK

APK: `Picsay_Pro_v2.3.3.1_1766148552_latestmodapks.com.apk`

Native entries found in the APK:

```
lib/x86/libGLImageOp.so
lib/armeabi-v7a/libGLImageOp.so
lib/x86/libImageOp.so
lib/armeabi-v7a/libImageOp.so
```

ELF headers:

| Path | ELF class | Machine |
|---|---|---|
| lib/x86/libGLImageOp.so | ELF32 | Intel 80386 |
| lib/x86/libImageOp.so | ELF32 | Intel 80386 |
| lib/armeabi-v7a/libGLImageOp.so | ELF32 | ARM |
| lib/armeabi-v7a/libImageOp.so | ELF32 | ARM |

Dynamic dependencies:

- x86/armeabi-v7a `libImageOp.so`: `libz.so`, `libjnigraphics.so`, `libstdc++.so`, `libc.so`, `libm.so`, `libdl.so`.
- x86/armeabi-v7a `libGLImageOp.so`: `libImageOp.so`, `libGLESv3.so`, `libEGL.so`, `libstdc++.so`, `libc.so`, `libm.so`, `libdl.so`.

Both libraries expose `JNI_OnLoad` and `JNI_OnUnload`. The JNI registration functions visible in the native symbol tables include `register_ImageConverter`, `register_ImageTask`, `register_PEStroke`, `register_TimImageProxy`, `register_fileRenderer`, `register_filter`, `register_scUtils`, and `register_PEWarp`.

The x86 and armeabi-v7a builds have matching application-level exported symbol names (343 common application exports in `libImageOp.so`, plus architecture-specific runtime helpers in the ARM build). This is strong evidence that the ARMv7 library is a useful reference for reconstructing the same native interface, but it is not an AArch64 binary.
