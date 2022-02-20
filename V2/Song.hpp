#pragma once

#include <string>
#include <SDL2/SDL_mixer.h>
#include <fstream>
#include "Enemy.hpp"


namespace v2
{
    class Song
    {
        std::string m_title, m_artist;
        float m_bpm;
        float m_duration;
        float m_playheadTime;
        int m_currentFrame;
        int m_totalFrames;

        Mix_Music* m_music;

        std::ifstream m_file;

        std::vector<Entity*> m_entities;

        void addEntity(Entity* e);
        void removeEntity(Entity* e);


    public:
        Song(Mix_Music* music, const std::string& title, const std::string& artist, float bpm, const std::string& file);
        Song();
        ~Song();

        inline void SetBPM(float bpm) { m_bpm = bpm; }
        inline float GetBPM() { return m_bpm; }

        float FrameToSec(int frame);
        int SecToFrame(float time); 
        static void SetVolume(float volume);

        void Play();

        void LoadEnts(cage::TextureManager& tm);

        void Update(float dt);
        std::vector<Entity*> GetNewSpawns();
        void Seek(float time);

        friend class Editor;
    };
}