#include "Playfield.hpp"
#include "Bullet.hpp"

namespace v2
{
    Playfield::Playfield(Song* song, cage::TextureManager& tm, int w, int h) : m_song(song), m_tm(tm)
    {
        song->LoadEnts(tm);
        Resize(w, h);
        m_player = new Player(tm);
        m_entities.push_back(m_player);
        song->Play();
    }

    void Playfield::Update(float dt)
    {
        m_song->Update(dt);
        if (auto spawns = m_song->GetNewSpawns(); !spawns.empty())
        {
            for (auto e : spawns)
            {
                m_entities.push_back(e);
                ((Enemy*)e)->m_Player = m_player;
            }
        }
        std::vector<Bullet*> toAdd;
        std::vector<Entity*> toKill;
        for (auto e : m_entities)
        {
            e->Update(dt);
            if (e->Dead)
                toKill.push_back(e);
            if (auto enemy = dynamic_cast<Enemy*>(e))
            {
                if (enemy->Shoot)
                {
                    float dx = m_player->x - enemy->x;
                    float dy = m_player->y - enemy->y;
                    float l = sqrt(dx * dx + dy * dy);
                    dx /= l;
                    dy /= l;
                    dx *= 600.0;
                    dy *= 600.0;

                    auto b = new Bullet(m_tm, dx, dy);
                    b->x = enemy->x;
                    b->y = enemy->y;
                    toAdd.push_back(b);
                    enemy->Shoot = false;
                }
            }
        }
        for (auto ba : toAdd)
            m_entities.push_back(ba);
        for (auto ek : toKill)
        {
            m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), ek), m_entities.end());
        }
    }

    void Playfield::Draw()
    {
        m_shader.Use();
        for (auto e : m_entities)
            e->Draw();
    }

    void Playfield::Resize(int w, int h)
    {
        m_shader.Use();
        m_shader.Projection->value = glm::ortho(0.f, static_cast<float>(w), static_cast<float>(h), 0.f);
        m_shader.Projection->ForwardToShader();
    }

    Playfield::~Playfield()
    {
        for (auto e : m_entities)
            delete e;
    }
}