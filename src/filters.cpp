#include "Filters.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <numbers>

namespace {
bool IsPositiveDigit(const std::string& s) {
    if (s.starts_with("-")) {
        return false;
    } else {
        for (size_t i = 0; i < s.size(); i++) {
            if (!std::isdigit(s[i])) {
                return false;
            }
        }
    }
    return true;
}
bool IsDouble(const std::string& s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '.') {
            continue;
        }
        if (!std::isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

double StringToDouble(const std::string& s) {
    return std::stod(s);
}

size_t StringToSizet(const std::string& s) {
    return std::stoull(s);
}

size_t GetIndexWithOffset(size_t index, int offset, size_t min, size_t max) {
    if (offset > 0 && offset + index >= max) {
        return max;
    } else if (offset < 0 && index < (-offset)) {
        return min;
    }
    return index + offset;
}

const Pixel& GetPixelByOffsetWidth(size_t h, size_t w, int offset, const Image& img) {
    return img.At(h, GetIndexWithOffset(w, offset, 0, img.Width() - 1));
}

const Pixel& GetPixelByOffsetHeight(size_t h, size_t w, int offset, const Image& img) {
    return img.At(GetIndexWithOffset(h, offset, 0, img.Height() - 1), w);
}
}  // namespace

Crop::Crop(size_t width, size_t height) : width_(width), height_(height) {
}

Image Crop::Apply(const Image& img) {
    Image result = Image(std::min(width_, img.Width()), std::min(height_, img.Height()));
    for (size_t h = 0; h < std::min(height_, img.Height()); h++) {
        for (size_t w = 0; w < std::min(width_, img.Width()); w++) {
            const Pixel& cur_pixel = img.At(h, w);
            result.Put(h, w, cur_pixel);
        }
    }
    return result;
}

std::unique_ptr<Filter> CreateCrop(const std::vector<std::string>& params) {
    if (params.size() != 2) {
        throw std::invalid_argument("Incorrect number of arguments for Crop filter");
    }
    if (!IsPositiveDigit(params[0]) || !IsPositiveDigit(params[1])) {
        throw std::invalid_argument("Non digit or non positive digit given like Crop param");
    }
    size_t width = StringToSizet(params[0]);
    size_t height = StringToSizet(params[1]);

    return std::make_unique<Crop>(width, height);
}

Image Grayscale::Apply(const Image& img) {
    const double red_pixel_weight = 0.299;
    const double green_pixel_weight = 0.587;
    const double blue_pixel_weight = 0.114;
    Image result = Image(img.Width(), img.Height());
    for (size_t h = 0; h < img.Height(); h++) {
        for (size_t w = 0; w < img.Width(); w++) {
            const Pixel& cur_pixel = img.At(h, w);
            double graycolor = red_pixel_weight * cur_pixel.red + green_pixel_weight * cur_pixel.green +
                               blue_pixel_weight * cur_pixel.blue;
            Pixel put = {graycolor, graycolor, graycolor};
            result.Put(h, w, put);
        }
    }
    return result;
}

std::unique_ptr<Filter> CreateGrayscale(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw std::invalid_argument("Incorrect number of arguments for Grayscale filter");
    }
    return std::make_unique<Grayscale>();
}

Image Negative::Apply(const Image& img) {
    Image result = Image(img.Width(), img.Height());
    for (size_t h = 0; h < img.Height(); h++) {
        for (size_t w = 0; w < img.Width(); w++) {
            const Pixel& cur_pixel = img.At(h, w);
            Pixel put = {1.0 - cur_pixel.red, 1.0 - cur_pixel.green, 1.0 - cur_pixel.blue};
            result.Put(h, w, put);
        }
    }
    return result;
}

std::unique_ptr<Filter> CreateNegative(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw std::invalid_argument("Incorrect number of arguments for Negative filter");
    }
    return std::make_unique<Negative>();
}

Matrix::Matrix(std::vector<double> weights) : weights_(weights) {
}

Image Matrix::Apply(const Image& img) {
    Image result = Image(img.Width(), img.Height());
    for (size_t h = 0; h < img.Height(); h++) {
        for (size_t w = 0; w < img.Width(); w++) {
            const Pixel& left_pixel = GetPixelByOffsetWidth(h, w, -1, img);
            const Pixel& right_pixel = GetPixelByOffsetWidth(h, w, 1, img);
            const Pixel& up_pixel = GetPixelByOffsetHeight(h, w, -1, img);
            const Pixel& down_pixel = GetPixelByOffsetHeight(h, w, 1, img);
            const Pixel& cur_pixel = img.At(h, w);
            Pixel put;
            put.red = weights_[2] * cur_pixel.red + weights_[0] * left_pixel.red + weights_[1] * right_pixel.red +
                      weights_[3] * up_pixel.red + weights_[4] * down_pixel.red;
            put.green = weights_[2] * cur_pixel.green + weights_[0] * left_pixel.green +
                        weights_[1] * right_pixel.green + weights_[3] * up_pixel.green + weights_[4] * down_pixel.green;
            put.blue = weights_[2] * cur_pixel.blue + weights_[0] * left_pixel.blue + weights_[1] * right_pixel.blue +
                       weights_[3] * up_pixel.blue + weights_[4] * down_pixel.blue;

            result.Put(h, w, put);
        }
    }
    return result;
}

Image Sharpening::Apply(const Image& img) {
    Matrix matrix_filter = Matrix({-1, -1, 5, -1, -1});  // NOLINT
    Image result = matrix_filter.Apply(img);
    return result;
}

std::unique_ptr<Filter> CreateSharpening(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw std::invalid_argument("Incorrect number of arguments for Sharpening filter");
    }
    return std::make_unique<Sharpening>();
}

EdgeDetection::EdgeDetection(double threshold) : threshold_(threshold) {
}

Image EdgeDetection::Apply(const Image& img) {
    Image grayscale_image = Grayscale().Apply(img);
    Matrix matrix_filter = Matrix({-1, -1, 4, -1, -1});  // NOLINT
    Image result = matrix_filter.Apply(grayscale_image);
    Pixel black = {0, 0, 0};
    Pixel white = {1, 1, 1};
    for (size_t h = 0; h < img.Height(); h++) {
        for (size_t w = 0; w < img.Width(); w++) {
            const Pixel& cur_pixel = result.At(h, w);
            if (cur_pixel.red > threshold_) {
                result.Put(h, w, white);
            } else {
                result.Put(h, w, black);
            }
        }
    }
    return result;
}

std::unique_ptr<Filter> CreateEdgeDetection(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw std::invalid_argument("Incorrect number of arguments for Edge Detecion filter");
    }
    if (!IsDouble(params[0])) {
        throw std::invalid_argument("Non double given like Crop param");
    }
    return std::make_unique<EdgeDetection>(StringToDouble(params[0]));
}

GaussianBlur::GaussianBlur(double sigma) : sigma_(sigma) {
    double weights_sum = 0;
    delta_ = 3 * static_cast<int>(sigma_);
    dist_weight_.resize(delta_ + 1);
    for (int distance = 0; distance <= delta_; distance++) {  // calculate weights for distance
        double weight = (1 / sigma_ * std::sqrt(2 * std::numbers::pi)) *
                        exp(-1 * ((distance) * (distance)) / (2 * sigma_ * sigma_));
        weights_sum += weight;
        if (distance == 0) {
            dist_weight_[distance] = weight;
        } else {
            dist_weight_[distance] = weight / 2;
        }
    }
    for (int distance = 0; distance <= delta_; distance++) {  // normalize weights
        dist_weight_[distance] /= weights_sum;
    }
}

Image GaussianBlur::Apply(const Image& img) {
    Image result = Image(img.Width(), img.Height());
    Image x_gauss = Image(img.Width(), img.Height());
    for (size_t h = 0; h < img.Height(); h++) {  // calculate gauss function for x only
        for (size_t w = 0; w < img.Width(); w++) {
            double red = 0;
            double green = 0;
            double blue = 0;
            for (int offset_w = -delta_; offset_w <= delta_; offset_w++) {
                const Pixel& temp = GetPixelByOffsetWidth(h, w, offset_w, img);
                red += temp.red * dist_weight_[std::abs(offset_w)];
                green += temp.green * dist_weight_[std::abs(offset_w)];
                blue += temp.blue * dist_weight_[std::abs(offset_w)];
            }
            Pixel cur_pixel = Pixel{red, green, blue};
            x_gauss.Put(h, w, cur_pixel);
        }
    }
    for (size_t h = 0; h < img.Height(); h++) {  // calculate result
        for (size_t w = 0; w < img.Width(); w++) {
            double red = 0;
            double green = 0;
            double blue = 0;
            for (int offset_h = -delta_; offset_h <= delta_; offset_h++) {
                const Pixel& temp = GetPixelByOffsetHeight(h, w, offset_h, x_gauss);
                red += temp.red * dist_weight_[std::abs(offset_h)];
                green += temp.green * dist_weight_[std::abs(offset_h)];
                blue += temp.blue * dist_weight_[std::abs(offset_h)];
            }
            Pixel cur_pixel = Pixel{red, green, blue};
            result.Put(h, w, cur_pixel);
        }
    }
    return result;
}

std::unique_ptr<Filter> CreateGaussianBlur(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw std::invalid_argument("Incorrect number of arguments for Gaussian Blur filter");
    }
    if (!IsDouble(params[0])) {
        throw std::invalid_argument("Non double given like Gaussian Blur param");
    }
    return std::make_unique<GaussianBlur>(StringToDouble(params[0]));
}

std::map<std::string, std::function<std::unique_ptr<Filter>(const std::vector<std::string>&)>> GetFilters() {
    return {{"crop", CreateCrop},        {"gs", CreateGrayscale},       {"neg", CreateNegative},
            {"sharp", CreateSharpening}, {"edge", CreateEdgeDetection}, {"blur", CreateGaussianBlur}};
}