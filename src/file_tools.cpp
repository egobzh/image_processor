#include "FileWorking.h"
#include "FileStructs.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace {
void FileReadBytes(char* pointer, std::ifstream& s, std::streamsize need) {
    s.read(pointer, need);
    if (s.gcount() < need) {
        throw std::invalid_argument("Invalid input BMP. Not enough bytes to read");
    }
}

uint32_t ReadUint32(std::ifstream& s) {
    unsigned char lst[4];
    FileReadBytes(reinterpret_cast<char*>(lst), s, 4);
    uint32_t result = static_cast<uint32_t>(lst[0]) + (static_cast<uint32_t>(lst[1]) << 8) +  // NOLINT
                      (static_cast<uint32_t>(lst[2]) << 16) +                                 // NOLINT
                      (static_cast<uint32_t>(lst[3]) << 24);                                  // NOLINT
    return result;
}

int32_t ReadInt32(std::ifstream& s) {
    return static_cast<int32_t>(ReadUint32(s));
}

void Int32toByte(char* mas, int32_t number) {
    uint32_t unumber = static_cast<uint32_t>(number);
    mas[0] = static_cast<char>(unumber & 0xFF);          // NOLINT
    mas[1] = static_cast<char>((unumber >> 8) & 0xFF);   // NOLINT
    mas[2] = static_cast<char>((unumber >> 16) & 0xFF);  // NOLINT
    mas[3] = static_cast<char>((unumber >> 24) & 0xFF);  // NOLINT
}

uint16_t ReadUint16(std::ifstream& s) {
    unsigned char lst[2];
    FileReadBytes(reinterpret_cast<char*>(lst), s, 2);
    uint16_t result = static_cast<uint16_t>(lst[0]) + (static_cast<uint16_t>(lst[1]) << 8);  // NOLINT
    return result;
}

int16_t ReadInt16(std::ifstream& s) {
    return static_cast<int16_t>(ReadUint16(s));
}

void Int16toByte(char* mas, int16_t number) {
    uint16_t unumber = static_cast<uint16_t>(number);
    mas[0] = static_cast<char>(unumber & 0xFF);         // NOLINT
    mas[1] = static_cast<char>((unumber >> 8) & 0xFF);  // NOLINT
}

uint8_t ReadUint8(std::ifstream& s) {
    unsigned char lst[1];
    FileReadBytes(reinterpret_cast<char*>(lst), s, 1);
    uint8_t result = static_cast<uint8_t>(lst[0]);
    return result;
}

BMPHeader ReadBMPHeader(std::ifstream& s) {
    BMPHeader result;
    FileReadBytes(result.magic, s, 2);
    result.file_size = ReadInt32(s);
    result.reserved[0] = ReadInt16(s);
    result.reserved[1] = ReadInt16(s);
    result.offset = ReadInt32(s);
    return result;
}

void CheckBmpHeadervalid(const BMPHeader header) {
    if (header.magic[0] != 'B' || header.magic[1] != 'M') {
        throw std::invalid_argument("Invalid input BMP. Not BM magic");
    }
}

int32_t CalculateRawBitmapData(const Image& img, int32_t depth) {
    return ((depth * static_cast<int32_t>(img.Width()) + 31) / 32) * 4 * static_cast<int32_t>(img.Height());  // NOLINT
}

BMPHeader GenerateBmpHeader(const Image& image) {
    const int32_t offset = 54;
    const int32_t color_depth = 24;
    BMPHeader result;
    result.magic[0] = 'B';
    result.magic[1] = 'M';
    result.file_size = CalculateRawBitmapData(image, color_depth) + offset;
    result.reserved[0] = 0;
    result.reserved[1] = 0;
    result.offset = offset;
    return result;
}

BMPinfoheader ReadBMPinfoheader(std::ifstream& s) {
    BMPinfoheader result;
    result.header_size = ReadInt32(s);
    result.width = ReadInt32(s);
    result.height = ReadInt32(s);
    result.color_planes = ReadInt16(s);
    result.depth = ReadInt16(s);
    result.compression = ReadInt32(s);
    result.raw_bitmap_data = ReadInt32(s);
    result.horizontal_resolution = ReadInt32(s);
    result.vertical_resolution = ReadInt32(s);
    result.colors_palette = ReadInt32(s);
    result.colors_used = ReadInt32(s);
    return result;
}

void CheckBmpInfoHeadervalid(const BMPinfoheader& header) {
    const int32_t color_depth = 24;
    if (header.width < 0 || header.height < 0) {
        throw std::invalid_argument("Invalid input BMP. Non positive width or height");
    }
    if (header.color_planes != 1) {
        throw std::invalid_argument("Invalid input BMP. color planes isn`t 1");
    }
    if (header.depth != color_depth) {
        throw std::invalid_argument("Invalid input BMP. Dont supported photo depth");
    }
    if (header.compression != 0) {
        throw std::invalid_argument("Invalid input BMP. Dont supported photo compression");
    }
    if (header.colors_palette != 0) {
        throw std::invalid_argument("Invalid input BMP. Dont supported color palette");
    }
}

BMPinfoheader GenerateBmpInfoHeader(const Image& image) {
    const int32_t dpi = 1337;
    const int32_t color_depth = 24;
    const int32_t header_size = 40;
    BMPinfoheader result;
    result.header_size = header_size;
    result.width = static_cast<int32_t>(image.Width());
    result.height = static_cast<int32_t>(image.Height());
    result.color_planes = 1;
    result.depth = color_depth;
    result.compression = 0;
    result.raw_bitmap_data = CalculateRawBitmapData(image, color_depth);
    result.horizontal_resolution = dpi;
    result.vertical_resolution = dpi;
    result.colors_palette = 0;
    result.colors_used = 0;
    return result;
}

Pixel ReadBmpPixel(std::ifstream& s) {
    const double max_uint8_size = 255.0;
    double blue = (static_cast<double>(ReadUint8(s))) / max_uint8_size;
    double green = (static_cast<double>(ReadUint8(s))) / max_uint8_size;
    double red = (static_cast<double>(ReadUint8(s))) / max_uint8_size;
    return Pixel{red, green, blue};
}

void WriteHeaders(BMPHeader header, BMPinfoheader infoheader, std::ofstream& s) {
    s.write(header.magic, 2);
    char cur_16[2];
    char cur_32[4];
    Int32toByte(cur_32, header.file_size);
    s.write(cur_32, 4);
    Int16toByte(cur_16, header.reserved[0]);
    s.write(cur_16, 2);
    Int16toByte(cur_16, header.reserved[1]);
    s.write(cur_16, 2);
    Int32toByte(cur_32, header.offset);
    s.write(cur_32, 4);

    Int32toByte(cur_32, infoheader.header_size);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.width);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.height);
    s.write(cur_32, 4);
    Int16toByte(cur_16, infoheader.color_planes);
    s.write(cur_16, 2);
    Int16toByte(cur_16, infoheader.depth);
    s.write(cur_16, 2);
    Int32toByte(cur_32, infoheader.compression);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.raw_bitmap_data);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.horizontal_resolution);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.vertical_resolution);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.colors_palette);
    s.write(cur_32, 4);
    Int32toByte(cur_32, infoheader.colors_used);
    s.write(cur_32, 4);
}

uint8_t ColorValueToInt(double value) {
    const double max_uint8_size = 255.0;
    return static_cast<uint8_t>(value * max_uint8_size);
}

void WritePixels(const Image& image, std::ofstream& s) {
    int32_t padding = ((4 - static_cast<int32_t>(image.Width()) * 3) % 4) & 3;  // NOLINT
    for (int32_t x = (static_cast<int32_t>(image.Height()) - 1); x >= 0; --x) {
        for (int32_t y = 0; y <= (static_cast<int32_t>(image.Width()) - 1); ++y) {
            Pixel cur_pixel = image.At(x, y);
            uint8_t blue = ColorValueToInt(cur_pixel.blue);
            uint8_t green = ColorValueToInt(cur_pixel.green);
            uint8_t red = ColorValueToInt(cur_pixel.red);
            char bgr[3] = {static_cast<char>(blue), static_cast<char>(green), static_cast<char>(red)};
            s.write(bgr, 3);
        }
        if (padding != 0) {
            char pad[3] = {0, 0, 0};
            s.write(pad, padding);
        }
    }
}

void CheckOpened(std::ifstream& input_file) {
    if (!input_file.is_open()) {
        throw std::invalid_argument("Cant open input file, try to check path");
    }
}

void CheckOpened(std::ofstream& output_file) {
    if (!output_file.is_open()) {
        throw std::invalid_argument("Cant open output file, try to check path");
    }
}
}  // namespace

Image ReadBMP(std::string& path) {
    std::ifstream input_file(path, std::ios::in | std::ios::binary);
    CheckOpened(input_file);
    BMPHeader header = ReadBMPHeader(input_file);
    CheckBmpHeadervalid(header);
    BMPinfoheader infoheader = ReadBMPinfoheader(input_file);
    CheckBmpInfoHeadervalid(infoheader);
    Image result = Image(infoheader.width, infoheader.height);
    int32_t padding = ((4 - infoheader.width * 3) % 4) & 3;  // NOLINT
    for (int32_t x = (infoheader.height - 1); x >= 0; --x) {
        for (int32_t y = 0; y <= (infoheader.width - 1); ++y) {
            Pixel pixel = ReadBmpPixel(input_file);
            result.Put(x, y, pixel);
        }
        if (padding != 0) {
            input_file.ignore(padding);
        }
    }

    input_file.close();
    return result;
}

void WriteBMP(const Image& image, std::string& path) {
    std::ofstream output_file(path, std::ios::out | std::ios::binary);
    CheckOpened(output_file);
    BMPHeader header = GenerateBmpHeader(image);
    BMPinfoheader infoheader = GenerateBmpInfoHeader(image);
    WriteHeaders(header, infoheader, output_file);
    WritePixels(image, output_file);
    output_file.close();
}