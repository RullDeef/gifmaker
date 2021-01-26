#include <cmath>
#include "renderer.hpp"

Renderer::Renderer(int width, int height)
    : Image(width, height)
{
    blendType = BlendType::MIX;
    lineWidth = 1.0f;
    strokeColor = { 0, 0, 0, 255 };
    fillColor = { 255, 255, 255, 255 };
}

void Renderer::Resize(int width, int height)
{
    this->width = width;
    this->height = height;

    delete[] this->data;
    this->data = new Pixel[GetSize()];
}

void Renderer::Clear()
{
    for (int i = 0; i < GetSize(); i++)
        data[i] = fillColor;
}

void Renderer::StrokeCircle(int x, int y, float radius)
{
    int radius2_1 = (radius - lineWidth / 2) * (radius - lineWidth / 2);
    int radius2_2 = (radius + lineWidth / 2) * (radius + lineWidth / 2);

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            int radius2 = (col - x) * (col - x) + (row - y) * (row - y);
            if (radius2_1 <= radius2 && radius2 <= radius2_2)
            {
                Pixel dst = data[row * width + col];
                data[row * width + col] = BlendPixels(dst, strokeColor);
            }
        }
    }
}

void Renderer::StrokeRect(int x, int y, int width, int height)
{
    for (int row = 0; row < this->height; row++)
    {
        for (int col = 0; col < this->width; col++)
        {
            int dx = std::min(std::abs(x - col), std::abs(x + width - col));
            int dy = std::min(std::abs(y - row), std::abs(y + height - row));
            if ((dx <= lineWidth / 2 && y - lineWidth / 2 <= row && row < y + height + lineWidth / 2) ||
                (dy <= lineWidth / 2 && x - lineWidth / 2 <= col && col < x + height + lineWidth / 2))
            {
                Pixel dst = data[row * this->width + col];
                data[row * this->width + col] = BlendPixels(dst, strokeColor);
            }
        }
    }
}

void Renderer::FillCircle(int x, int y, float radius)
{
    int radius2 = radius * radius;

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if ((col - x) * (col - x) + (row - y) * (row - y) <= radius2)
            {
                Pixel dst = data[row * width + col];
                data[row * width + col] = BlendPixels(dst, fillColor);
            }
        }
    }
}

void Renderer::FillRect(int x, int y, int width, int height)
{
    for (int row = (y > 0 ? y : 0); row < this->height && row < y + height; row++)
    {
        for (int col = (x > 0 ? x : 0); col < this->width && col < x + width; col++)
        {
            Pixel dst = data[row * this->width + col];
            data[row * this->width + col] = BlendPixels(dst, fillColor);
        }
    }
}

void Renderer::BeginPath()
{
    path.clear();
}

void Renderer::ClosePath()
{
    if (path.size() > 0)
    {
        path.push_back(path.front());
        path.back().type = PathPoint::Type::LINE;
    }
}

void Renderer::Stroke()
{
    for (int i = 0; i < path.size(); i++)
    {
        if (path[i].type == PathPoint::Type::LINE)
            RasterLine({ path[i - 1].x, path[i - 1].y }, { path[i].x, path[i].y });
        else if (path[i].type == PathPoint::Type::ARC_CENTER)
        {
            PathPoint arcData = path[i + 1];
            FloatPoint center = { path[i].x, path[i].y };
            float startAngle = atan2f(path[i - 1].y - center.y, path[i - 1].x - center.x);
            if (startAngle < 0.0f)
                startAngle += 6.28318530718f;
            float endAngle = startAngle + arcData.angle;
            RasterArc(center, arcData.radius, startAngle, endAngle);
            i += 2;
        }
    }
}

void Renderer::Fill()
{
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // check if point is inside rendering path
            int counter = 0; // intersects counter. Odd == inside

            // max forward offset (pixels that can be safely classified as current)
            int maxOffset = width - col;

            {
                FloatPoint targetPoint = { (float)col, (float)row };
                const FloatPoint targetDir = { 1.0f, 0.0f };

                // loop for rays intersections
                for (int i = 1; i < path.size(); i++)
                {
                    if (path[i].type == PathPoint::Type::LINE)
                    {
                        FloatPoint lineStart = { path[i - 1].x, path[i - 1].y };
                        FloatPoint lineEnd = { path[i].x, path[i].y };
                        FloatPoint lineDir = lineEnd - lineStart;

                        if (lineDir.y != 0.0f)
                        {
                            // start & end points lay on the different sides by the ray
                            if ((lineStart.y < targetPoint.y && targetPoint.y <= lineEnd.y) ||
                                (lineEnd.y < targetPoint.y && targetPoint.y <= lineStart.y))
                            {
                                if (lineStart.x >= targetPoint.x || lineEnd.x >= targetPoint.x)
                                {
                                    FloatPoint intersection;
                                    float portion = (targetPoint.y - lineStart.y) / lineDir.y;

                                    intersection.x = lineStart.x + portion * lineDir.x;
                                    intersection.y = targetPoint.y;

                                    if (intersection.x > targetPoint.x)
                                    {
                                        counter++;
                                        maxOffset = std::min<int>(maxOffset, intersection.x - targetPoint.x);
                                    }
                                }
                            }
                        }
                    }
                    else if (path[i].type == PathPoint::Type::ARC_CENTER)
                    {
                        FloatPoint center = { path[i].x, path[i].y };
                        float radius = path[i + 1].radius;

                        if (center.y - radius < targetPoint.y && targetPoint.y < center.y + radius)
                        {
                            float deltaAngle = path[i + 1].angle;
                            FloatPoint startPoint = { path[i - 1].x, path[i - 1].y };

                            float startAngle = atan2f(path[i - 1].y - center.y, path[i - 1].x - center.x);
                            if (startAngle < 0.0f)
                                startAngle += 6.28318530718f;
                            float endAngle = startAngle + deltaAngle;

                            FloatPoint endPoint;
                            endPoint.x = center.x + radius * cos(endAngle);
                            endPoint.y = center.y + radius * sin(endAngle);

                            // first semicircle intersection
                            float distToTarget2 = (targetPoint.x - center.x) * (targetPoint.x - center.x) +
                                (targetPoint.y - center.y) * (targetPoint.y - center.y);

                            if (distToTarget2 >= radius * radius && targetPoint.x < center.x)
                            {
                                // possible intersection
                                bool hasIntersection = false;

                                if (startPoint.y < endPoint.y)
                                {
                                    if (startPoint.x < center.x)
                                        hasIntersection = targetPoint.y < startPoint.y ||
                                            (endPoint.x < center.x && endPoint.y < targetPoint.y);
                                    else if (endPoint.x < center.x)
                                        hasIntersection = endPoint.y < targetPoint.y;
                                }
                                else
                                {
                                    if (startPoint.x < center.x && endPoint.x < center.x)
                                        hasIntersection = startPoint.y < targetPoint.y && targetPoint.y < endPoint.y;
                                    else if (startPoint.x < center.x && endPoint.x >= center.x)
                                        hasIntersection = startPoint.y < targetPoint.y;
                                    else if (startPoint.x >= center.x && endPoint.x < center.x)
                                        hasIntersection = targetPoint.y < endPoint.y;
                                    else
                                        hasIntersection = true;
                                }

                                if (hasIntersection)
                                {
                                    counter++;
                                    // update offset
                                    float distToCircle = center.x - targetPoint.x - sqrtf(radius * radius -
                                        (targetPoint.y - center.y) * (targetPoint.y - center.y));
                                    maxOffset = std::min<int>(maxOffset, distToCircle);
                                }
                            }

                            // second semicircle intersection
                            if (distToTarget2 <= radius * radius || targetPoint.x < center.x)
                            {
                                // possible intersection
                                bool hasIntersection = false;

                                if (startPoint.y < endPoint.y)
                                {
                                    if (startPoint.x > center.x && endPoint.x > center.x)
                                        hasIntersection = startPoint.y < targetPoint.y && targetPoint.y < endPoint.y;
                                    else if (startPoint.x > center.x && endPoint.x <= center.x)
                                        hasIntersection = startPoint.y < targetPoint.y;
                                    else if (startPoint.x <= center.x && endPoint.x > center.x)
                                        hasIntersection = targetPoint.y < endPoint.y;
                                    else
                                        hasIntersection = true;
                                }
                                else
                                {
                                    hasIntersection = startPoint.x > center.x && targetPoint.y > startPoint.y ||
                                                      endPoint.x > center.x && targetPoint.y < endPoint.y;
                                }

                                if (hasIntersection)
                                {
                                    counter++;
                                    // update offset
                                    float distToCircle = center.x - targetPoint.x + sqrtf(radius * radius -
                                        (targetPoint.y - center.y) * (targetPoint.y - center.y));
                                    maxOffset = std::min<int>(maxOffset, distToCircle);
                                }
                            }
                        }
                    }
                }

            }

            if (counter % 2 == 1)
            {
                // blend pixels
                for (int offset = 0; offset < maxOffset; offset++)
                {
                    Pixel dst = data[row * width + col + offset];
                    data[row * width + col + offset] = BlendPixels(dst, fillColor);
                }
            }

            // jump over to unclassified pixel
            col += maxOffset;
        }
    }
}

void Renderer::MoveTo(FloatPoint point)
{
    if (path.size() == 0 || path.back().type != PathPoint::Type::START)
        path.push_back({ PathPoint::Type::START, point.x, point.y });
    else
        path.back() = { PathPoint::Type::START, point.x, point.y };
}

void Renderer::LineTo(FloatPoint point)
{
    if (path.size() == 0)
        path.push_back({ PathPoint::Type::START, point.x, point.y });
    else
        path.push_back({ PathPoint::Type::LINE, point.x, point.y });
}

void Renderer::Arc(FloatPoint center, float radius, float startAngle, float endAngle, bool clockwise)
{
    if (!clockwise)
    {
        float temp = startAngle;
        startAngle = endAngle;
        endAngle = temp;
    }

    while (startAngle < 0.0f)
        startAngle += 6.283185482f;
    while (startAngle >= 6.283185482f)
        startAngle -= 6.283185482f;

    while (endAngle < 0.0f)
        endAngle += 6.283185482f;
    while (endAngle >= 6.283185482f)
        endAngle -= 6.283185482f;
    
    if (endAngle <= startAngle)
        endAngle += 6.283185482f;

    float deltaAngle = endAngle - startAngle;

    FloatPoint startPoint;
    startPoint.x = center.x + radius * cos(startAngle);
    startPoint.y = center.y + radius * sin(startAngle);

    FloatPoint endPoint;
    endPoint.x = center.x + radius * cos(endAngle);
    endPoint.y = center.y + radius * sin(endAngle);

    if (path.size() == 0)
        path.push_back({ PathPoint::Type::START, startPoint.x, startPoint.y });
    else
    {
        if (clockwise)
            path.push_back({ PathPoint::Type::LINE, startPoint.x, startPoint.y });
        else
        {
            path.push_back({ PathPoint::Type::LINE, endPoint.x, endPoint.y });
            path.push_back({ PathPoint::Type::START, startPoint.x, startPoint.y });
        }
    }

    path.push_back({ PathPoint::Type::ARC_CENTER, center.x, center.y });
    path.push_back({ PathPoint::Type::ARC_DATA, radius, deltaAngle });
    
    if (clockwise)
        path.push_back({ PathPoint::Type::ARC_END, endPoint.x, endPoint.y });
    else
        path.push_back({ PathPoint::Type::ARC_END, startPoint.x, startPoint.y });
}

Pixel Renderer::BlendPixels(Pixel dst, Pixel src)
{
    switch (blendType)
    {
        case BlendType::OVERLAY:
            return src;

        case BlendType::ADD:
            return dst + src;

        case BlendType::SUBTRACT:
            return dst - src;

        case BlendType::MULTIPLY:
            return dst * src;

        case BlendType::MIX:
            float src_a = src.a / 255.0f;
            int r = dst.r * (1.0f - src_a) + src.r * src_a;
            int g = dst.g * (1.0f - src_a) + src.g * src_a;
            int b = dst.b * (1.0f - src_a) + src.b * src_a;
            return { (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
    }

    return { 0, 0, 0, 0 };
}

void Renderer::RasterLine(FloatPoint pointFrom, FloatPoint pointTo)
{
    float A = pointTo.y - pointFrom.y;
    float B = pointFrom.x - pointTo.x;
    float C = -A * pointFrom.x + (-B) * pointFrom.y;

    float A2B2 = hypot(A, B);

    float dx = pointTo.x - pointFrom.x;
    float dy = pointTo.y - pointFrom.y;

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            float d = abs(A * col + B * row + C) / A2B2;
            bool pointOnLine = d <= lineWidth / 2;

            // closer than end points
            bool passPointFrom = dx * (col - pointFrom.x) + dy * (row - pointFrom.y) >= 0.0f;
            bool passPointTo = -dx * (col - pointTo.x) - dy * (row - pointTo.y) >= 0.0f;

            if (pointOnLine && passPointFrom && passPointTo)
            {
                Pixel dst = data[row * width + col];
                data[row * width + col] = BlendPixels(dst, strokeColor);
            }
        }
    }
}

void Renderer::RasterArc(FloatPoint center, float radius, float startAngle, float endAngle)
{
    float radius2_1 = (radius - lineWidth / 2) * (radius - lineWidth / 2);
    float radius2_2 = (radius + lineWidth / 2) * (radius + lineWidth / 2);

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            float dx = col - center.x;
            float dy = row - center.y;
            float r2 = dx * dx + dy * dy;
            float angle = atan2f(dy, dx);
            while (angle < startAngle)
                angle += 6.28318530718f;

            bool onArc = radius2_1 <= r2 && r2 <= radius2_2;

            if (onArc && startAngle <= angle && angle < endAngle)
            {
                Pixel dst = data[row * width + col];
                data[row * width + col] = BlendPixels(dst, strokeColor);
            }
        }
    }
}
