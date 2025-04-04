#include "Image.h"
#include <string>

Image ReadBMP(std::string& path);
void WriteBMP(const Image& image, std::string& path);