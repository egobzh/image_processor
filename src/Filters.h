#include "Image.h"
#include <string>
#include <memory>
#include <map>
#include <functional>

class Filter {
public:
    virtual Image Apply(const Image& img) = 0;
    virtual ~Filter() = default;
};

class Crop : public Filter {
private:
    size_t width_;
    size_t height_;

public:
    Crop(size_t width, size_t height);
    Image Apply(const Image& img) override;
};

class Grayscale : public Filter {
public:
    Image Apply(const Image& img) override;
};

class Negative : public Filter {
public:
    Image Apply(const Image& img) override;
};

class Matrix : public Filter {
private:
    std::vector<double> weights_;

public:
    explicit Matrix(std::vector<double> weights);
    Image Apply(const Image& img) override;
};

class Sharpening : public Filter {
public:
    Image Apply(const Image& img) override;
};

class EdgeDetection : public Filter {
private:
    double threshold_;

public:
    explicit EdgeDetection(double threshold);
    Image Apply(const Image& img) override;
};

class GaussianBlur : public Filter {
private:
    double sigma_;
    std::vector<double> dist_weight_;
    int delta_;

public:
    explicit GaussianBlur(double sigma);
    Image Apply(const Image& img) override;
};

std::unique_ptr<Filter> CreateCrop(const std::vector<std::string>& params);
std::unique_ptr<Filter> CreateGrayscale(const std::vector<std::string>& params);
std::unique_ptr<Filter> CreateNegative(const std::vector<std::string>& params);
std::unique_ptr<Filter> CreateSharpening(const std::vector<std::string>& params);
std::unique_ptr<Filter> CreateEdgeDetection(const std::vector<std::string>& params);
std::unique_ptr<Filter> CreateGaussianBlur(const std::vector<std::string>& params);

std::map<std::string, std::function<std::unique_ptr<Filter>(const std::vector<std::string>&)>> GetFilters();