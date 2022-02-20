#pragma once

#include <vector>
#include "Song.hpp"
#include "GameShader.hpp"
#include "Player.hpp"

namespace v2
{
    class Playfield
    {
        std::vector<Entity*> m_entities;
        Song* m_song;
        Player* m_player;
        cage::TextureManager& m_tm;
        
        GameShader m_shader;
    public:
        Playfield(Song* song, cage::TextureManager& tm, int w, int h);
        ~Playfield();

        void Resize(int w, int h);
        void Update(float dt);
        void Draw();
    };
}
