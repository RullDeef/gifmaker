#include <iostream>
#include <cmath>
#include "scene.hpp"

AnimatorParams params;

void Scene::Update(float dt)
{
}

void Scene::Render(float time)
{
    if (time < 1.0 / 30)
    {
        renderer.fillColor = { 0, 0, 0, 255 };
        renderer.Clear();
    }
    else
    {
        renderer.fillColor = { 0, 0, 0, 25 };
        renderer.FillRect(0, 0, params.imageWidth, params.imageHeight);
    }

    renderer.fillColor = { 0, 255, 255, 255 };
    for (float real_time = time; real_time - time < 5.0f; real_time += 1.0f)
    {
        for (int dy = 150; dy < params.imageHeight + 50; dy += 25)
        {
            for (int dx = 0; dx < params.imageWidth; dx += params.imageWidth / 10)
            {
                float a = real_time / 5 - 0.8 * dy / params.imageHeight + dx / params.imageWidth;
                float x = (a - floorf(a)) * params.imageWidth;
                a = (x - (float)dx / params.imageWidth) / 50 * 3.1415 + 2 * a;
                float xx = asinf(sinf(a + 2 * real_time));
                xx /= 3.1415;
                float y = dy - 100.0f * sqrtf(1 - xx * xx);

                renderer.FillCircle(x, y, 2);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    params.imageWidth = 320;
    params.imageHeight = 240;
    params.prepTime = 1.0f;

    Scene(params).Execute();
    return 0;
}
