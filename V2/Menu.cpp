#include "Menu.hpp"
#include <Glad/glad/glad.h>

namespace v2
{

    Menu::Menu(cage::Game& game) : cage::GameState(game)
    {

    }

    Menu::~Menu()
    {
    }

    void Menu::OnRevealed()
    {
    }

    void Menu::OnHidden()
    {
    }

    void Menu::ProcessEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                quit();
                break;
            }
        }
    }

    void Menu::Update(float dt)
    {
    }

    void Menu::Draw()
    {
        glClearColor(0.08, 0.63, 0.33, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

}