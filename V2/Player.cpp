#include "Player.hpp"

namespace v2
{
    Player::Player(cage::TextureManager& tm) : Entity(tm, "player")
    {

    }

    void Player::Update(float dt)
    {
        auto keys = SDL_GetKeyboardState(nullptr);
        float dx = 0, dy = 0;
        if (keys[SDL_SCANCODE_W])
            dy -= 1;
        if (keys[SDL_SCANCODE_A])
            dx -= 1;
        if (keys[SDL_SCANCODE_S])
            dy += 1;
        if (keys[SDL_SCANCODE_D])
            dx += 1;

        const float speed = 600.f;

        x += speed * dx * dt;
        y += speed * dy * dt;
    }
}