#include "Image.h"
#include <algorithm>

Image::Image(size_t width, size_t height) {
    Pixel white = Pixel{1, 1, 1};
    width_ = width;
    height_ = height;
    pixels_.resize(width * height, white);
}

const Pixel& Image::At(size_t x, size_t y) const {
    return pixels_[x * width_ + y];
}

void Image::Put(size_t x, size_t y, const Pixel& pixel) {
    pixels_[x * width_ + y] =
        Pixel{std::clamp(pixel.red, 0.0, 1.0), std::clamp(pixel.green, 0.0, 1.0), std::clamp(pixel.blue, 0.0, 1.0)};
}

size_t Image::Width() const {
    return width_;
}

size_t Image::Height() const {
    return height_;
}