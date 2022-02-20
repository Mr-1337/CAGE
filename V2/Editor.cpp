#include "Editor.hpp"
#include "UI/PlaybackBar.hpp"

#include <Core/Game.hpp>
#include <Graphics/UI/ScrollField.hpp>

namespace v2
{

    Editor::Editor(cage::Game& game) : cage::GameState(game), m_playing(false), m_metroTimer(0.f), m_metronome(false), m_clickSound(*game.GetSoundManager().Get("click.ogg")), m_lastFrame(0)
    {
        auto [w, h] = game.GetWindow().GetSize();
        glViewport(0, 0, w, h);
        cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, static_cast<float>(w), static_cast<float>(h), 0.f);
        cage::ui::UIElement::shader->Projection->ForwardToShader();
        m_root.Resize({ w, h });
        m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
        m_timeline = std::make_shared<cage::ui::UIElement>();
        m_timeline->SetRelativeSizeAxes(cage::ui::Axis::HORIZONTAL);
        
        m_timeline->SetColor({ 0.7, 0.2, 0.3, 1.0 });
        m_timeline->Resize({ 1.0f, 150.f });
        m_timeline->SetParentMounting(cage::ui::MountPoint::TOP);
        m_timeline->SetLocalMounting(cage::ui::MountPoint::TOP);
        auto box = std::make_shared<cage::ui::UIElement>();
        box->Resize({ 500.f, 300.f });
        box->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
        box->MoveTo({ 600.f, 600.f });
        m_root.Add(box);
        m_root.Add(m_timeline);

        auto playbackTools = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout(glm::vec2{ 0.f, 0.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));
        auto buttons = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout());
        auto tex = game.GetTextureManager();
        auto play = std::make_shared<cage::ui::Button>(tex.Get("Play.png"), std::nullopt, std::nullopt);
        auto pause = std::make_shared<cage::ui::Button>(tex.Get("Pause.png"), std::nullopt, std::nullopt);
        auto stop = std::make_shared<cage::ui::Button>(tex.Get("Stop.png"), std::nullopt, std::nullopt);
        auto metro = std::make_shared<cage::ui::Button>(tex.Get("Metronome.png"), std::nullopt, std::nullopt);
        buttons->Add(play);
        buttons->Add(pause);
        buttons->Add(stop);
        buttons->Add(metro);
        auto sf = std::make_shared<cage::ui::ScrollField>();
        sf->OnUpdate = [this](float bpm)
        {
            m_song->SetBPM(bpm);
        };

        buttons->Add(sf);

        play->OnClick = [this]()
        {
            m_playing = true;
        };

        pause->OnClick = [this]()
        {
            m_playing = false;
        };

        stop->OnClick = [this]()
        {
            m_playing = false;
            m_metroTimer = 0.f;
        };

        metro->OnClick = [this]()
        {
            m_metronome = !m_metronome;
        };

        playbackTools->Add(std::make_shared<PlaybackBar>(w * 0.75f));
        playbackTools->Add(buttons);
        playbackTools->SetParentMounting(cage::ui::MountPoint::TOP);
        playbackTools->SetLocalMounting(cage::ui::MountPoint::TOP);

        m_root.Add(playbackTools);

        m_song = std::make_unique<Song>();
        m_playfield = std::make_unique<Playfield>(m_song.get(), game.GetTextureManager(), w, h);
    }

    Editor::~Editor()
    {

    }

    void Editor::ProcessEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            cage::Event event = m_input.Convert(e);
            if (m_root.HandleEvent(event))
                return;
            switch (e.type)
            {
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    glViewport(0, 0, e.window.data1, e.window.data2);
                    cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, static_cast<float>(e.window.data1), static_cast<float>(e.window.data2), 0.f);
                    cage::ui::UIElement::shader->Projection->ForwardToShader();
                    m_root.Resize({ e.window.data1, e.window.data2 });
                    m_playfield->Resize(e.window.data1, e.window.data2);
                }
                break;
            case SDL_QUIT:
                quit();
                break;
            }
        }
    }

    void Editor::Update(float dt)
    {
        if (m_playing)
        {
            m_metroTimer += dt;
            if (m_song->SecToFrame(m_metroTimer) / 32 != m_lastFrame)
            {
                m_lastFrame = m_song->SecToFrame(m_metroTimer) / 32;
                if (m_metronome)
                    getGame().GetPlaybackEngine().Play(m_clickSound, 2);
            }
            m_playfield->Update(dt);
        }
        m_root.Update(dt);
    }

    void Editor::Draw()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_playfield->Draw();
        m_root.shader->Use();
        m_root.Draw();
    }

    void Editor::OnHidden()
    {

    }

    void Editor::OnRevealed()
    {

    }

}