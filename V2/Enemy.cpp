#include "Enemy.hpp"

namespace v2
{
    Enemy::Enemy(cage::TextureManager& tm, float bpm) : Entity(tm, "enemy"), k_attackRate(60.f / bpm), m_Player(nullptr), Shoot(false), timer(0)
    {
        vx = rand() % 100 / 100.f - 0.5;
        vy = rand() % 100 / 100.f;
        float l = sqrt(vx * vx + vy * vy);
        vx /= l;
        vy /= l;
        vx *= 500.0f;
        vy *= 700.0f;
    }

    void Enemy::Update(float dt)
    {
        x += vx * dt;
        y += vy * dt;
        timer += dt;
        if (timer > k_attackRate)
        {
            Shoot = true;
            timer = 0;
            std::cout << "Shooting\n" << x << " " << y << "\n";
        }
        if (y > 1200.f || x < -100.0f || x > 2500.0f)
            Dead = true;
    }
}