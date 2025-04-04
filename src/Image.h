#pragma once
#include <cstdint>
#include <vector>

struct Pixel {
    double red;
    double green;
    double blue;
};

class Image {
private:
    size_t width_;
    size_t height_;
    std::vector<Pixel> pixels_;

public:
    Image(size_t width, size_t height);
    const Pixel& At(size_t x, size_t y) const;
    void Put(size_t x, size_t y, const Pixel& pixel);
    size_t Width() const;
    size_t Height() const;
};