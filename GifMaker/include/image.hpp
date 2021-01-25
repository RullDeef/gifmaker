#pragma once

#include <cctype>

struct Pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Pixel operator+(const Pixel& other);
    Pixel operator-(const Pixel& other);
    Pixel operator*(const Pixel& other);
};

using Color = Pixel;

class Image
{
public:
    Image(int width, int height);
    Image(const Image& copy);
    ~Image();

    static Image Load(const char *filename);
    void Save(const char *filename);

    void SetPixel(int row, int col, Pixel pixel);
    Pixel GetPixel(int row, int col, Pixel defaultPixel = { 0, 0, 0, 0 }) const;

protected:
    inline bool IsInside(int row, int col) const
    {
        return 0 <= row && row < height && 0 <= col && col < width;
    }

    inline int GetSize() const
    {
        return width * height;
    }

protected:
    int width;
    int height;
    Pixel *data;
};
