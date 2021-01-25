#pragma once

#include "animator.hpp"

class Scene : public Animator
{
public:
    Scene(AnimatorParams params)
        : Animator(params)
    {}

protected:
    void Update(float dt) override;
    void Render(float time) override;
};
