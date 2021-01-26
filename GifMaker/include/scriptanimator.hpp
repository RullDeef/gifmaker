#pragma once

#include "duktape.h"
#include "animator.hpp"

class ScriptAnimator
{
public:
    ScriptAnimator(const char *jsSource);

    void Execute();

private:
    void Update(float dt);
    void Render(float time);
    void PrepareDuktapeEnv();

public:
    AnimatorParams params;
    Renderer renderer;

private:
    const char *jsSource;
    duk_context *ctx;
};
