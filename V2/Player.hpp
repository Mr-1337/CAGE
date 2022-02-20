#pragma once

#include "Entity.hpp"

namespace v2
{
    class Player : public Entity
    {
    public:
        Player(cage::TextureManager& tm);
        void Update(float dt) override;
    };
}