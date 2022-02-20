#pragma once

#include <filesystem>
#include <Graphics/UI/LayoutGroup.hpp>
#include <Graphics/UI/FlowLayout.hpp>
#include "MenuButton.hpp"
#include "../Song.hpp"

namespace v2
{
    class SongWheel : public cage::ui::LayoutGroup
    {
        cage::Font m_font;
        std::vector<Song*> m_songs;
        std::shared_ptr<cage::ui::UIElement> m_bg;
        Song* m_selected;
    public:
        SongWheel(cage::ui::UIElement* root) : cage::ui::LayoutGroup(new cage::ui::FlowLayout({ 0.f, 2.0f }, cage::ui::FlowLayout::Orientation::VERTICAL, false)), m_font("Assets/Fonts/impact.ttf", 30), m_selected(nullptr)
        {
            std::vector<std::shared_ptr<cage::ui::UIElement>> backgrounds, buttons;
            std::filesystem::path songDir = "Assets\\Songs";
            for (auto const& dir : std::filesystem::directory_iterator{ songDir })
            {
                if (dir.is_directory())
                {
                    std::cout << dir.path() << std::endl;
                    std::filesystem::path file = dir.path() / std::string("info.txt");

                    std::cout << file << std::endl;

                    std::ifstream info;
                    info.open(file.string());

                    std::string title, artist;
                    float bpm;
                    std::getline(info, title);
                    std::getline(info, artist);

                    info >> bpm;
                    std::cout << bpm << std::endl;

                      
                    auto songBox = std::make_shared<MenuButton>(title + " - " + artist, m_font);
                    songBox->SetColor({ 0.8f, .4f, 0.5f, 0.7f });
                    songBox->Resize({ 600.f, 150.f });

                    auto texture = std::make_shared<cage::Texture>(IMG_Load((dir.path().string() + "/" + "bg.png").c_str()));

                    auto bg = std::make_shared<cage::ui::UIElement>();
                    bg->SetActiveTexture(texture);

                    backgrounds.push_back(bg);
                    bg->SetVisible(false);

                    auto audio = Mix_LoadMUS((dir.path().string() + "/" + "song.mp3").c_str());
                    auto song = new Song(audio, title, artist, bpm, (dir.path() / std::string("chart.txt")).string());

                    songBox->OnHover = [this, song, bg, songBox]()
                    {
                        song->Play();
                        if (m_bg)
                            m_bg->SetVisible(false);
                        bg->SetVisible(true);
                        m_bg = bg;

                        songBox->HaltAllTransforms();
                        songBox->ScheduleTransform(std::make_unique<cage::ui::transforms::FadeTo>(glm::vec4{ 0.9f, .7f, 0.8f, 0.7f }, 0.f, 0.2f));

                    };

                    songBox->OnUnHover = [this, song, bg, songBox]()
                    {
                        songBox->HaltAllTransforms();
                        songBox->ScheduleTransform(std::make_unique<cage::ui::transforms::FadeTo>(glm::vec4{ 0.8f, .4f, 0.5f, 0.7f }, 0.f, 0.2f));
                    };

                    songBox->OnRelease = [this, song, root, backgrounds]()
                    {
                        //for (auto b : backgrounds)
                            //root->Remove(b);
                        m_selected = song;
                    };

                    buttons.push_back(songBox);
                    
                }
            }

            for (auto bg : backgrounds)
            {
                bg->SetRelativeSizeAxes(cage::ui::Axis::BOTH);
                bg->Resize({ 1.0f, 1.0f });
                root->Add(bg);
            }

            for (auto b : buttons)
            {
                Add(b);
            }
        }

        Song* GetSelected()
        {
            return m_selected;
        }

    };
}