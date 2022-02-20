#pragma once

#include <Graphics/UI/Dragable.hpp>

namespace v2
{
    class PlaybackBar : public cage::ui::Dragable
    {
        std::shared_ptr<cage::ui::UIElement> m_playhead, m_bar;
        float m_length;

    public:
        PlaybackBar(float length) : cage::ui::Dragable(std::nullopt, std::nullopt), m_length(length)
        {
            Resize({ length, 32.f });
            m_bar = std::make_shared<cage::ui::UIElement>();
            m_bar->SetColor({ 1.f, 1.f, 1.f, 1.f });
            m_bar->SetRelativeSizeAxes(cage::ui::Axis::BOTH);
            m_bar->Resize({ 1.0f, .25f });
            
            m_playhead = std::make_shared<cage::ui::UIElement>();
            m_playhead->SetColor({ 0.f, 1.f, 0.f, 1.f });
            m_playhead->SetRelativeSizeAxes(cage::ui::Axis::VERTICAL);
            m_playhead->Resize({ 20.f, .75f });
            m_playhead->SetRelativePositionAxes(cage::ui::Axis::HORIZONTAL);
            m_playhead->SetParentMounting(cage::ui::MountPoint::CENTER_LEFT);
            Add(m_bar);
            Add(m_playhead);

            OnDrag = [this](glm::vec2 v)
            {
                float p = (m_playhead->GetPosition().x + v.x) / GetSize().x;
                p = std::clamp(p, 0.f, 1.f);
                SetProgress(p);
            };

            

        }

        void SetProgress(float progress)
        {
            m_playhead->MoveTo({ progress, 0.f });
        }

    private:

        bool onClick(int x, int y) override
        {
            if (cage::ui::Dragable::onClick(x, y))
            {
                auto p = toLocalSpace(x, y).x;
                p += 0.5f;
                SetProgress(p);
                return true;
            }
            
            return false;
        }

    };
}