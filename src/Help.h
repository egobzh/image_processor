#include <iostream>

void WriteHelp() {
    std::cout << "This is an image processor which working with 24-bits images." << std::endl;
    std::cout << "Example for command line args:" << std::endl;
    std::cout << "image_processor {path to input file} {path to outpit file} [-{filter name 1} [filters param 1] "
                 "[filters param 2] ...] ..."
              << std::endl;
    std::cout << "Available filters and its params: " << std::endl;
    std::cout << "1)Crop (-crop width height)" << std::endl;
    std::cout << "  Crops the image to the specified width and height. The upper left part of the image is used."
              << std::endl;
    std::cout << "2)Grayscale (-gs)" << std::endl;
    std::cout << "  Converts the image to shades of gray." << std::endl;
    std::cout << "3)Negative (-neg)" << std::endl;
    std::cout << "  Converts the image to a negative." << std::endl;
    std::cout << "4)Sharpening (-sharp)" << std::endl;
    std::cout << "  Increase the sharpness." << std::endl;
    std::cout << "5)Edge Detection (-edge threshold)" << std::endl;
    std::cout << "  Border selection." << std::endl;
    std::cout << "6)Gaussian Blur (-blur sigma)" << std::endl;
    std::cout << "  Blur by Gaussian alghoritm." << std::endl;
}