#include <cstdio>
#include <cstdlib>
#include "animator.hpp"

Animator::Animator(AnimatorParams params)
    : params(params), renderer(params.imageWidth, params.imageHeight)
{}

void Animator::Execute()
{
    system("mkdir -p .super-temp-dir");

    float time = 0.0f;

    Render(time);
    while (time < params.prepTime)
    {
        time += 1.0f / params.frameRate;
        Update(1.0f / params.frameRate);
        Render(time);
    }

    for (int i = 0; i < params.framesCount; i++)
    {
        time += 1.0f / params.frameRate;
        Update(1.0f / params.frameRate);
        Render(time);

        char tempFilename[32];
        snprintf(tempFilename, 32, ".super-temp-dir/%03i.png", i);
        renderer.Save(tempFilename);
    }

    system("convert -delay 1.5 -loop 0 .super-temp-dir/*.png out.gif");
    system("rm -rf .super-temp-dir");
}
