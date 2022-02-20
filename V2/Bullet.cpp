#include "Bullet.hpp"

namespace v2
{
    Bullet::Bullet(cage::TextureManager& tm, float vx, float vy) : Entity(tm, "glow"), vx(vx), vy(vy)
    {

    }

    void Bullet::Update(float dt)
    {
        x += vx * dt;
        y += vy * dt;
        if (y <= -20.f || y >= 1500.f)
            Dead = true;
    }
}