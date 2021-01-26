#pragma once

#include <vector>
#include "image.hpp"

struct FloatPoint
{
    float x;
    float y;

    inline FloatPoint operator+(const FloatPoint& other) const
    {
        return { x + other.x, y + other.y };
    }

    inline FloatPoint operator-(const FloatPoint& other) const
    {
        return { x - other.x, y - other.y };
    }

    // dot product
    inline float operator*(const FloatPoint& other) const
    {
        return x * other.x + y * other.y;
    }

    // cross product
    inline float operator^(const FloatPoint& other) const
    {
        return x * other.y - y * other.x;
    }
};

struct PathPoint
{
    enum class Type
    {
        START,
        LINE,
        ARC_CENTER,
        ARC_DATA,
        ARC_END
    } type;

    union
    {
        float x;      // line
        float radius; // arc
    };

    union
    {
        float y;     // line
        float angle; // arc
    };
};

class Renderer : public Image
{
public:
    enum class BlendType
    {
        MIX,
        ADD,
        SUBTRACT,
        OVERLAY,
        MULTIPLY
    };

    Renderer(int width = 1, int height = 1);

    void Resize(int width, int height);

    // fills entire image with fill color
    // works regardless of render type
    void Clear();

    // fast immediate mode rendering
    void StrokeCircle(int x, int y, float radius);
    void StrokeRect(int x, int y, int width, int height);
    void FillCircle(int x, int y, float radius);
    void FillRect(int x, int y, int width, int height);

    // Paths rendering
    void BeginPath();
    void ClosePath();
    void Stroke();
    void Fill();

    void MoveTo(FloatPoint point);
    void LineTo(FloatPoint point);
    void Arc(FloatPoint center, float radius, float startAngle = 0.0f, float endAngle = 6.283185482f, bool clockwise = true);

    // returns copy of current image
    Image Snapshoot() const;

private:
    Pixel BlendPixels(Pixel dst, Pixel src);
    void RasterLine(FloatPoint pointFrom, FloatPoint pointTo);

public:
    void RasterArc(FloatPoint center, float radius, float startAngle, float endAngle);
    BlendType blendType;
    float lineWidth;
    Color strokeColor;
    Color fillColor;

private:
    std::vector<PathPoint> path;
};
