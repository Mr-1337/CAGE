#pragma once

#include "Entity.hpp"

namespace v2
{
    class Bullet : public Entity
    {
        float vx, vy;
    public:
        Bullet(cage::TextureManager& tm, float vx, float vy);
        void Update(float dt) override;
    };
}