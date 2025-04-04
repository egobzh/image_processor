#include <cstdint>

struct BMPHeader {
    char magic[2];
    int32_t file_size;
    int16_t reserved[2];
    int32_t offset;
};

struct BMPinfoheader {
    int32_t header_size;
    int32_t width;
    int32_t height;
    int16_t color_planes;
    int16_t depth;
    int32_t compression;
    int32_t raw_bitmap_data;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    int32_t colors_palette;
    int32_t colors_used;
};