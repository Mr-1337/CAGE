#pragma once

#include "../LayoutGroup.hpp"
#include "../GridLayout.hpp"
#include "../FlowLayout.hpp"
#include "../ListLayout.hpp"
#include "../Text.hpp"
#include "../ScrollPanel.hpp"
#include "../../../IO/InputManager.hpp"

#include <format>

namespace cage
{
	namespace ui
	{
		/*
		* InputDebugger provides helpful visual tools for showing input state.
		* Features an onscreen keyboard as well as showing registered controls
		*/
		class InputDebugger : public LayoutGroup
		{
			// A single on screen key
			class Key : public UIElement
			{
				std::shared_ptr<Text> m_value;
			public:
				Key(glm::vec2 size, std::string str, Font& font)
				{
					Resize(size);
					auto text = std::make_shared<Text>(font);
					m_value = std::make_shared<Text>(font);
					text->SetText(str);
					text->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
					m_value->SetText("Nothing");
					m_value->MoveTo({ 0.f, 30.f });
					m_value->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
					Add(text);
					Add(m_value);
				}
				void SetValue(glm::vec3 val)
				{
					glm::vec3 color = val * 0.5f + glm::vec3{ 0.5f, 0.5f, 0.5f };
					SetColor({ color, 0.7f });
					std::string str = std::format("{{{:+.3f},{:+.3f},{:+.3f}}}", val.x, val.y, val.z);
					m_value->SetText(str);
				}
			};

			Font m_font;
			std::map<std::string, std::shared_ptr<Key>> m_keys;
			io::InputManager& m_input;

		public:
			InputDebugger(io::InputManager& input) : LayoutGroup(new GridLayout({10.f, 10.f}, 3)), m_font("Assets/Fonts/consola.ttf", 12), m_input(input)
			{
				for (auto& control : input)
				{
					auto key = std::make_shared<Key>(glm::vec2{ 150.f, 80.f }, control.first, m_font);
					Add(key);
					m_keys[control.first] = key;
				}
			}

			void Update(float dt) override
			{
				for (auto& control : m_input)
				{
					m_keys[control.first]->SetValue(control.second);
				}
			}
		};
	}
}