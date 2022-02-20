#pragma once

#include <Core/GameState.hpp>
#include <Graphics/UI/GridLayout.hpp>
#include <Graphics/UI/ScrollPanel.hpp>
#include <Audio/Sound.hpp>

#include "Playfield.hpp"

namespace v2
{

    class Editor : public cage::GameState
    {
        cage::ui::UIElement m_root;
        std::shared_ptr<cage::ui::UIElement> m_timeline;
        std::unique_ptr<Song> m_song;
        std::unique_ptr<Playfield> m_playfield;

        bool m_playing, m_metronome;

        float m_metroTimer;
        int m_lastFrame;

        cage::audio::Sound& m_clickSound;

    public:

        Editor(cage::Game& game);
        ~Editor();

        void ProcessEvents() override;
        void Update(float dt) override;
        void Draw() override;

        void OnRevealed() override;
        void OnHidden() override;
    };

}
