#pragma once

#include <list>
#include "renderer.hpp"

struct AnimatorParams
{
    int imageWidth;
    int imageHeight;

    int framesCount;
    float prepTime;
    float frameRate;
    const char* outputFileName;

    AnimatorParams()
        : imageWidth(100), imageHeight(100),
        framesCount(60), frameRate(60.0f),
        prepTime(0.0f), outputFileName("out.gif")
    {}
};

class Animator
{
public:
    Animator(AnimatorParams params = AnimatorParams());

    void Execute();

protected:
    virtual void Update(float dt)
    {}

    virtual void Render(float time)
    {}

protected:
    AnimatorParams params;
    Renderer renderer;
};
