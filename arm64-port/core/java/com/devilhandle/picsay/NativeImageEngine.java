package com.devilhandle.picsay;

import android.graphics.Bitmap;

/**
 * Standalone JNI facade for the reconstructed ARM64 CPU image engine.
 * Requires RGBA_8888 mutable Bitmaps and libpicsay_engine.so.
 * This facade is not the legacy PicSay JNI ABI.
 */
public final class NativeImageEngine {
    static { System.loadLibrary("picsay_engine"); }
    private NativeImageEngine() {}
    public static native boolean invert(Bitmap bitmap);
    public static native boolean grayscale(Bitmap bitmap);
    public static native boolean sharpen(Bitmap bitmap, float amount);
    public static native boolean blur(Bitmap bitmap, int radius);
    public static native boolean sepia(Bitmap bitmap, float amount);
    public static native boolean temperature(Bitmap bitmap, float shift);
    public static native boolean pixelize(Bitmap bitmap, int blockSize);
    public static native boolean bulge(Bitmap bitmap, float cx, float cy, float radius, float strength);
    public static native boolean twirl(Bitmap bitmap, float cx, float cy, float radius, float radians);
    public static native boolean adjust(Bitmap bitmap, float brightness, float contrast, float saturation);
    public static native boolean vignette(Bitmap bitmap, float strength, float radius);
    public static native boolean blend(Bitmap destination, Bitmap source, int x, int y, float opacity);
    public static native boolean heatMap(Bitmap bitmap, float intensity);
    public static native boolean mirror(Bitmap bitmap, boolean horizontal, boolean vertical);
    public static native boolean quantizeLuminance(Bitmap bitmap, int levels);
    public static native boolean boost(Bitmap bitmap, float amount);
    public static native boolean multiplyColor(Bitmap bitmap, int rgba, float amount);
}
