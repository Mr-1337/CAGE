#pragma once

#include <functional>
#include "Hoverable.hpp"
#include "Text.hpp"

namespace cage
{
    namespace ui
    {
        class ScrollField : public Hoverable
        {
            float m_value;
            std::shared_ptr<Text> m_text;
        public:

            std::function<void(float)> OnUpdate;

            ScrollField() : Hoverable(std::nullopt, std::nullopt), m_value(100.f)
            {
                m_text = std::make_shared<Text>();
                m_text->SetText(std::to_string(m_value));
                Add(m_text);
                Resize(m_text->GetSize());
            }

            bool HandleEvent(cage::Event& e) override
            {
                if (Hoverable::HandleEvent(e))
                    return true;
                if (auto se = std::get_if<cage::ScrollEvent>(&e))
                {
                    if (hovering())
                    {
                        m_value += se->amount;
                        m_text->SetText(std::to_string(m_value));
                        if (OnUpdate)
                            OnUpdate(m_value);
                        return true;
                    }
                }
                return false;
            }
        };
    }
}