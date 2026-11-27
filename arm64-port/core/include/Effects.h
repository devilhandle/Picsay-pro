#pragma once
#include "ImageEngine.h"
namespace picsay {
bool temperature(Image&, float kelvinShift);
bool sepia(Image&, float amount);
bool colorMatrix(Image&, const float matrix4x5[20]);
bool sobel(Image&, float strength);
bool vignette(Image&, float strength, float radius);
bool twirl(Image&, float cx, float cy, float radius, float radians);
bool spherize(Image&, float cx, float cy, float radius, float strength);
bool linearStretch(Image&, float cx, float cy, float halfWidth, float scale);
bool circleSplash(Image&, float cx, float cy, float radius, int segments);
bool mixColor(Image&, uint32_t rgba, float amount);
}

bool heatMap(Image&, float intensity);
bool mirror(Image&, bool horizontal, bool vertical);
bool quantizeLuminance(Image&, int levels);
bool boost(Image&, float amount);
bool multiplyColor(Image&, uint32_t rgba, float amount);
