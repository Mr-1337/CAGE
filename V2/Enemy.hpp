#pragma once

#include "Entity.hpp"
#include "Player.hpp"


namespace v2
{
    class Enemy : public Entity
    {
        float vx;
        float vy;
        const float k_attackRate;
        float timer;
    public:
        bool Shoot;
        Enemy(cage::TextureManager& tm, float bpm);
        Player* m_Player;
        void Update(float dt) override;
    };
}