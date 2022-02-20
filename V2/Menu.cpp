#include "Menu.hpp"
#include <Glad/glad/glad.h>
#include <Core/Game.hpp>

#include <Graphics/UI/FlowLayout.hpp>
#include <Graphics/UI/LayoutGroup.hpp>
#include "UI/PlaybackBar.hpp"
#include "Editor.hpp"

namespace v2
{

    Menu::Menu(cage::Game& game) : cage::GameState(game), m_playfield(nullptr)
    {
        auto [w, h] = getGame().GetWindow().GetSize();
        m_root.Resize({ w, h });
        cage::ui::UIElement::shader->Use();
        cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, static_cast<float>(w), static_cast<float>(h), 0.f);
        cage::ui::UIElement::shader->Projection->ForwardToShader();
        m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
        cage::Font font("Assets/Fonts/impact.ttf", 72);
        auto play = std::make_shared<MenuButton>("Play", font);
        auto edit = std::make_shared<MenuButton>("Edit", font);
        auto quitButton = std::make_shared<MenuButton>("Quit", font);
        auto layout = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout({ 0.f, 10.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));

        layout->Add(play);
        layout->Add(edit);
        layout->Add(quitButton);

        Song::SetVolume(0.05f);

        quitButton->OnRelease = [this]()
        {
            quit();
        };

        int w2 = w;
        int h2 = h;

        play->OnRelease = [this, layout, w2, h2]()
        {
            layout->SetVisible(false);
            m_scrollPane = std::make_shared<cage::ui::ScrollPanel>(glm::vec2{ w2 , h2 * 0.75f });
            m_songWheel = std::make_shared<SongWheel>(&m_root);
            m_scrollPane->Add(m_songWheel);
            m_root.Add(m_scrollPane);
        };

        edit->OnRelease = [this]()
        {
            s_stateMachine->Push(new Editor(getGame()));
        };

        m_root.Add(layout);


        glViewport(0, 0, w, h);
    }

    Menu::~Menu()
    {
    }

    void Menu::OnRevealed()
    {
        auto winSize = getGame().GetWindow().GetSize();
        m_root.Resize(glm::vec2{ winSize.first, winSize.second });
    }

    void Menu::OnHidden()
    {
    }

    void Menu::ProcessEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            auto converted = m_input.Convert(e);
            if (m_root.HandleEvent(converted))
                return;
            switch (e.type)
            {
            case SDL_QUIT:
                quit();
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    int w = e.window.data1;
                    int h = e.window.data2;
                    cage::ui::UIElement::shader->Use();
                    cage::ui::UIElement::shader->Projection->value = glm::ortho(0.f, static_cast<float>(w), static_cast<float>(h), 0.f);
                    cage::ui::UIElement::shader->Projection->ForwardToShader();
                    m_root.Resize({ w, h });
                    glViewport(0, 0, w, h);
                }
                break;
            }
        }
    }

    void Menu::Update(float dt)
    {
        m_root.Update(dt);
        if (m_songWheel && m_songWheel->GetSelected() != nullptr)
        {
            auto [w, h] = getGame().GetWindow().GetSize();
            m_playfield = new Playfield(m_songWheel->GetSelected(), getGame().GetTextureManager(), w, h);
            m_root.Remove(m_scrollPane);
            m_songWheel = nullptr;
        }
        if (m_playfield)
            m_playfield->Update(dt);
    }

    void Menu::Draw()
    {
        glClearColor(0.08, 0.63, 0.33, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        cage::ui::UIElement::shader->Use();
        m_root.Draw();

        if (m_playfield)
        {
            m_playfield->Draw();
        }

    }

}