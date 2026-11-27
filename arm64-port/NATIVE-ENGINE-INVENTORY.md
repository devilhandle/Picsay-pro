# Native engine inventory — armeabi-v7a reference build

Source: `lib/armeabi-v7a/libImageOp.so` from the supplied APK. This is a static symbol-table inventory, not source code and not proof that each export is called by the Java layer.

## Library roles

- `libImageOp.so`: core tiled-image decode/processing pipeline, bitmap transfer, filter graph, image export, brush rendering, and JNI bindings.
- `libGLImageOp.so`: OpenGL ES / EGL accelerated warp path; dynamically depends on `libImageOp.so`.

## Core image pipeline symbols observed

- Decode and image representation: `TimDecoder_allocObject`, `TimDecoder_setTim`, `TimDecoder_decode2`, `TimDecoder_decode3`, `TimDecoder_reset`, `TimDecoder_runFilters2`, `TimDecoder_runFiltersTiled`, `TimDecoder_optimizeTiling`.
- Bitmap interop: `ImageInterleaved_initWithBitmap`, `ImageInterleaved_initWithAndLockBitmap`, `ImageInterleaved_unlockBitmap`, `ImageInterleaved_lockBitmap`, `TimImageProxy_copyFromBitmap`, `TimImageProxy_copyToBitmap`, `TimImageProxy_sampleToBitmap`.
- File input/output: `FilterInputPNG_read`, `FilterOutputJPEG_initWithOutput_quality`, `FilterOutputPNG_initWithOutput_pixelFormat`, `FileOutput_initWithFile`, `FileOutput_writeBytes`.
- Filter graph and scheduling: `FilterNode_initWithIO`, `TimDecoder_addFilter`, `TimDecoder_insertFilter_atIndex`, `TimDecoder_replaceFilterIO_withFilterIO`, `TimDecoder_reuseFilterIOs`, `TimDecoder_filterIOneedsFullImage`, `TimDecoder_doesFilterNeedTileAccess`.

## Effect setup exports observed

These are exported setup functions in the ARMv7 reference library:

- `setupEffectDefault`
- `setupEffectSharpen`
- `setupEffectFauxHDR`
- `setupEffectXray`
- `setupEffectGritty`
- `setupEffectMixPreFilteredColors`
- `setupEffectHistoryPaint`
- `setupEffectWarp`
- `setupEffectPixelize`
- `setupEffectGrayscale`
- `setupEffectInvert`
- `setupEffectColorMatrix`
- `setupEffectBlendWithColor`
- `setupEffectDropShadow`
- `setupEffectTemperature`
- `setupEffectChannelMap`
- `setupEffectHeatMap`
- `setupEffectColorize`
- `setupEffectRainbow`
- `setupEffectMirror`
- `setupEffectOutOfFocus`
- `setupEffectDistortCircleSplash`
- `setupEffectDistortLinearStretch`
- `setupEffectDistortStretch`
- `setupEffectDistortBulge`
- `setupEffectDistortTwirl`
- `setupEffectDistortSpherize`
- `setupEffectPopArt`
- `setupEffectComicPrint`
- `setupEffectQuantizateLuminance`
- `setupEffectNeon`
- `setupEffectPencilSketch`
- `setupEffectSoftGlow`
- `setupEffectFilmGrain`
- `setupEffectBoost`
- `setupEffectSmooth`
- `setupEffectBlur`
- `setupEffectRedscale`
- `setupEffectMultiplyWithColor`
- `setupEffectMixWithColor`
- `setupEffectDefaultAlphaBlending`
- `setupEffectDefaultPreFilteredAlphaBlending`
- `setupEffectDistort`

## Other algorithm families observed

The exported function names also expose implementation families for:

- PNG/JPEG decode and encode, pixel unpacking/packing, RGB/Y'CbCr conversion.
- Tiled image buffers, file-backed memory regions, and filter IO reuse.
- Normal, premultiplied, plus, multiply, alpha-union and mask blending.
- Blur kernels, box blur, unsharp mask, sharpen, Sobel edge detection, glow, film grain, gritty desaturation, halftone, pop-art, pencil sketch, rainbow and color mapping.
- Crop, rotate, pixelize, warp and geometric distortion.
- Brush rasterization through `renderBrush_CircleAA` and `renderBrush_CircleSmoothSquared`.

## What this means for an ARM64 reconstruction

This is not a small JNI shim. The native engine contains a multi-stage filter graph and specialized pixel/bitmap code. A functional AArch64 port must preserve its data formats, tiling rules, alpha conventions, filter ordering, cancellation behavior, and file output semantics. The OpenGL warp library adds a second implementation path with GL/EGL state requirements.

The ARMv7 binary is a useful reference for behavioral comparison and reverse engineering. It cannot be used as an AArch64 shared object, and copying symbol names into empty functions would not recreate the engine.
