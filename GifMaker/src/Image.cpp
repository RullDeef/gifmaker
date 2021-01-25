#include <string>
#include <stdexcept>
#include "Image.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

Pixel Pixel::operator+(const Pixel &other)
{
    int r = this->r + other.r;
    int g = this->g + other.g;
    int a = this->a + other.a;
    int b = this->b + other.b;

    r = (r < 256 ? r : 255);
    g = (g < 256 ? g : 255);
    b = (b < 256 ? b : 255);
    a = (a < 256 ? a : 255);

    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

Pixel Pixel::operator-(const Pixel &other)
{
    int r = this->r - other.r;
    int g = this->g - other.g;
    int b = this->b - other.b;
    int a = this->a - other.a;

    r = (r >= 0 ? r : 0);
    g = (g >= 0 ? g : 0);
    b = (b >= 0 ? b : 0);
    a = (a >= 0 ? a : 0);

    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

Pixel Pixel::operator*(const Pixel &other)
{
    int r = this->r * other.r;
    int g = this->g * other.g;
    int b = this->b * other.b;
    int a = this->a * other.a;

    r = (r < 256 ? r : 255);
    g = (g < 256 ? g : 255);
    b = (b < 256 ? b : 255);
    a = (a < 256 ? a : 255);

    return { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
}

Image::Image(int width, int height)
    : width(width), height(height)
{
    data = new Pixel[GetSize()];
}

Image::Image(const Image &copy)
    : width(copy.width), height(copy.height)
{
    data = new Pixel[GetSize()];
    
    for (unsigned int i = 0; i < GetSize(); i++)
        data[i] = copy.data[i];
}

Image::~Image()
{
    delete[] data;
}

Image Image::Load(const char *filename)
{
    Image result(1, 1);
    delete[] result.data;

    int channels;
    result.data = (Pixel *)stbi_load(filename, &result.width, &result.height, &channels, 4);
    return result;
}

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void Image::Save(const char *filename)
{
    std::string filestr = filename;

    if (ends_with(filestr, ".jpg") || ends_with(filestr, ".jpeg"))
        stbi_write_jpg(filename, width, height, 4, data, 100);
    else if (ends_with(filestr, ".png"))
        stbi_write_png(filename, width, height, 4, data, 4 * width);
    else
        throw std::runtime_error("bad filename");
}

void Image::SetPixel(int row, int col, Pixel pixel)
{
    if (IsInside(row, col))
        data[row * width + col] = pixel;
}

Pixel Image::GetPixel(int row, int col, Pixel defaultPixel) const
{
    if (IsInside(row, col))
        return data[row * width + col];
    else
        return defaultPixel;
}
