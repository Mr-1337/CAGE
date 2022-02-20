#pragma once

#include <Graphics/UI/Button.hpp>
#include <Graphics/UI/Text.hpp>
#include <Graphics/UI/Transforms.hpp>

namespace v2
{
    class MenuButton : public cage::ui::Button
    {
        std::shared_ptr<cage::ui::Text> m_text;
        const cage::Font& m_font;
    public:
        MenuButton(const std::string& name, cage::Font& font) : cage::ui::Button(std::nullopt, std::nullopt, std::nullopt), m_font(font)
        {
            m_text = std::make_shared<cage::ui::Text>(m_font);
            //m_text->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
            m_text->SetText(name);
            Add(m_text);
            Resize({ 300.f, m_text->GetSize().y });

            const glm::vec4 color = { 0.65f, 0.1f, 0.1f, 1.0f };
            SetColor(color);

            OnHover = [&, color]()
            {
                HaltAllTransforms();
                glm::vec4 targetColor = glm::clamp(color * 1.5f, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f, 1.f });
                ScheduleTransform(std::make_unique<cage::ui::transforms::FadeTo>(targetColor, 0.0f, 0.2f));
            };

            OnUnHover = [&, color]()
            {
                HaltAllTransforms();
                ScheduleTransform(std::make_unique<cage::ui::transforms::FadeTo>(color, 0.0f, 0.2f));
            };

            OnClick = [&, color]()
            {
                HaltAllTransforms();
                glm::vec4 confirmColor = glm::clamp(color * 0.85f, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f, 1.f });
                confirmColor.a = 1.0f;
                ScheduleTransform(std::make_unique <cage::ui::transforms::FadeTo>(confirmColor, 0.0f, 0.2f));
            };
        }
    };
}