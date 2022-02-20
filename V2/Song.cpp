#include "Song.hpp"

namespace v2
{
    float Song::FrameToSec(int frame)
    {
        // bpm * min * 32 = frame
        // min = frame / (32 * bpm)
        // sec = min * 60
        return static_cast<float>(frame) / (32.f * m_bpm) * 60.f;
    }

    int Song::SecToFrame(float time)
    {
        return 32 * m_bpm * (time / 60.f);
    }

    void Song::SetVolume(float volume)
    {
        if (volume >= 0.f && volume <= 1.0f)
            Mix_VolumeMusic(255 * volume);
        else
            throw std::out_of_range("Volume must be withing [0.0, 1.0]");
    }

    Song::Song(Mix_Music* music, const std::string& title, const std::string& artist, float bpm, const std::string& file) :
        m_title(title),
        m_artist(artist),
        m_bpm(bpm),
        m_music(music),
        m_playheadTime(0.f),
        m_currentFrame(0)
    {
        m_file.open(file);
    }

    Song::Song() :
        m_title("Untitled Song"),
        m_artist("Unknown Artist"),
        m_bpm(120.f),
        m_music(nullptr),
        m_playheadTime(0.f),
        m_currentFrame(0)
    {

    }

    void Song::addEntity(Entity* e)
    {
        m_entities.push_back(e);
    }

    void Song::removeEntity(Entity* e)
    {
        m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e));
        delete e;
    }

    void Song::LoadEnts(cage::TextureManager& tm)
    {
        for (int i = 0; i < 412; i++)
        {
            auto e = new Enemy(tm, m_bpm);

            e->Init(400.f, -150.0f, i * 64);
            m_entities.push_back(e);

            auto f = new Enemy(tm, m_bpm);
            f->Init(400.f, -150.0f, i * 32);
            m_entities.push_back(f);
        }
    }

    Song::~Song()
    {
        if (m_music)
            Mix_FreeMusic(m_music);
    }

    void Song::Seek(float time)
    {
        m_playheadTime = time;
        Mix_RewindMusic();
        Mix_SetMusicPosition(time);
    }

    std::vector<Entity*> Song::GetNewSpawns()
    {
        std::vector<Entity*> ents;
        for (auto e : m_entities)
        {
            if (!e->Added && e->GetFrame() <= m_currentFrame)
            {
                ents.push_back(e);
                e->Added = true;
            }
        }
        return ents;
    }

    void Song::Play()
    {
        Mix_PlayMusic(m_music, 0);
    }

    void Song::Update(float dt)
    {
        m_playheadTime += dt;
        
        // beats / minute * minute = beat
        // beat * 32 = frame

        m_currentFrame = SecToFrame(m_playheadTime);
    }
}