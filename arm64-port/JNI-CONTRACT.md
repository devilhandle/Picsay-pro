# JNI contract extracted from classes.dex

Source APK: `Picsay_Pro_v2.3.3.1_1766148552_latestmodapks.com.apk`

This list is extracted from DEX class-data records, not guessed from native exports. The access flags include `ACC_NATIVE`. Descriptors use standard Dalvik/Java descriptor notation.

## Native methods

| Class | Method | Descriptor |
|---|---|---|
| `PSDocumentRendererToFile` | `nativeRender` | `(IIII[BIIIFILjava/lang/String;Landroid/graphics/Bitmap;)Z` |
| `SCImageTask` | `nativeCancel` | `(I)V` |
| `SCImageConverter` | `convertBitmap` | `(Landroid/graphics/Bitmap;IFII)Z` |
| `SCImageConverter` | `convertImageProxy` | `(IIIFFII)Z` |
| `SCImageFilter` | `blend_layer_toBitmap` | `(IFIIIIIIIIIIIIILandroid/graphics/Bitmap;)Z` |
| `SCImageFilter` | `filter_alphaBitmap` | `(Ljava/lang/Object;IFFLandroid/graphics/Bitmap;Landroid/graphics/Bitmap;)Z` |
| `SCImageFilter` | `fixRedEye` | `(IIIFFFI)I` |
| `SCImageFilter` | `blend_layer` | `(IFIIIIIIIII)Z` |
| `SCImageFilter` | `nativeFilterImageProxy` | `(Ljava/lang/Object;IFFIIIII)Z` |
| `PEStroke` | `addPoint` | `(IFFFI)Z` |
| `PEStroke` | `ctor` | `(II)I` |
| `PEStroke` | `dtor` | `(I)V` |
| `PEStroke` | `initFromByteArray_withSize` | `([BI)I` |
| `PEStroke` | `pointsCount` | `(I)I` |
| `PEStroke` | `renderBrushPreview_toBitmap` | `(IIFFIFLandroid/graphics/Bitmap;FFF)V` |
| `PEStroke` | `renderBrush_toImageProxy` | `(IIIIFFIFIFFFF[F)I` |
| `PEStroke` | `setUsedValues_samplingScale_maximumSize` | `(IIFI)I` |
| `PEStroke` | `writeToByteArray_withSize` | `(I[BI)I` |
| `PEWarpSession` | `GPU_draw` | `(IIIFFFF)V` |
| `PEWarpSession` | `acquireImage` | `(II)V` |
| `PEWarpSession` | `copyToImage` | `(II)V` |
| `PEWarpSession` | `ctor` | `()I` |
| `PEWarpSession` | `dtor` | `(I)V` |
| `PEWarpSession` | `releaseImage` | `(II)V` |
| `PEWarpSession` | `renderWarp` | `(IIIIFFIFF[F)I` |
| `PEWarpSession` | `setHistory` | `(II)V` |
| `PEWarpSession` | `updateArea_withImage` | `(IIIIII)V` |
| `PEWarpSession` | `willUpdateArea_clear` | `(IIIIIZ)V` |
| `PEWarpSession` | `initWithSize` | `(IIILandroid/graphics/Bitmap;[F)Z` |
| `TimImageProxy` | `blendBitmap_withBackgroundColor_rect` | `(Landroid/graphics/Bitmap;IIIII)V` |
| `TimImageProxy` | `blend_withColor_maskImageProxy` | `(IIII)V` |
| `TimImageProxy` | `copyFromBitmap` | `(IILandroid/graphics/Bitmap;I)Z` |
| `TimImageProxy` | `copyToBitmap` | `(IILandroid/graphics/Bitmap;I)Z` |
| `TimImageProxy` | `fillWithColor` | `(IIIIII)V` |
| `TimImageProxy` | `invert` | `(I)V` |
| `TimImageProxy` | `isGray` | `(I)Z` |
| `TimImageProxy` | `nativeConstructor` | `()I` |
| `TimImageProxy` | `nativeDestructor` | `(I)V` |
| `TimImageProxy` | `nativeLoadFromFile` | `(ILjava/lang/String;I)Z` |
| `TimImageProxy` | `nativeSaveToFile` | `(ILjava/lang/String;I)Z` |
| `TimImageProxy` | `premultiplyWithAlpha` | `(III)V` |
| `TimImageProxy` | `sample` | `(IIFFFFFFF)Z` |
| `TimImageProxy` | `sampleColor` | `(IIFFFF)I` |
| `TimImageProxy` | `sampleToBitmap` | `(IIFFFFFLandroid/graphics/Bitmap;FFI)Z` |
| `TimImageProxy` | `setDefaultTimForSize` | `(IIIIII)Z` |

Total: **45 native declarations**.

## Native registration classes and library split

- `libImageOp.so`: `TimImageProxy`, `SCImageConverter`, `SCImageTask`, `SCImageFilter`, `PSDocumentRendererToFile`, `PEStroke`.
- `libGLImageOp.so`: `PEWarpSession`; it depends on `libImageOp.so`, `libGLESv3.so`, and `libEGL.so`.

## Porting implications

1. Preserve every class, method name, descriptor, static/instance calling convention, and native registration contract.
2. Reconstruct native pointer ownership and lifecycle for `ctor`/`dtor`, `nativeConstructor`/`nativeDestructor`, and `acquireImage`/`releaseImage`.
3. Preserve Android bitmap pixel format, row-stride, lock/unlock, and JNI reference rules.
4. Preserve the GL/EGL context assumptions in `PEWarpSession`; this cannot be replaced by a CPU-only stub without changing behavior.
5. Validate real image load → edit → render → save flows on an AArch64 Android device.

The JNI declarations define the interface, not the complete implementation. This document does not claim that a native ARM64 implementation has been reconstructed.
