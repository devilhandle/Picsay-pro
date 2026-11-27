#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace picsay {
struct Image {
    int width=0, height=0;
    std::vector<uint8_t> rgba;
    bool valid() const { return width>0 && height>0 && rgba.size()==size_t(width)*height*4; }
};
bool invert(Image&);
bool grayscale(Image&);
bool adjust(Image&, float brightness, float contrast, float saturation);
bool boxBlur(Image&, int radius);
bool sharpen(Image&, float amount);
bool blend(Image& dst, const Image& src, int x, int y, float opacity);
bool crop(const Image&, int x, int y, int w, int h, Image& out);
bool rotate90(const Image&, int clockwiseQuarterTurns, Image& out);
bool pixelize(Image&, int blockSize);
bool warpBulge(Image&, float cx, float cy, float radius, float strength);
}
