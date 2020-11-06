#pragma once

#include "../../CAGE/Graphics/UI/Button.hpp"
#include "../../CAGE/Graphics/UI/Text.hpp"

namespace ub
{
	class EditorTab : public cage::ui::Button
	{
	public:
		EditorTab(const std::string& label, const cage::Font& font) :
			cage::ui::Button(std::nullopt, std::nullopt, std::nullopt)
		{
			auto text = std::make_shared<cage::ui::Text>(font);
			text->SetColor({ 255, 255, 255, 255 });
			text->SetText(label);
			auto width = text->GetSize().x * 1.2;
			auto height = text->GetSize().y * 1.2;
			Resize({ width, height });
			Add(text);
			SetColor({ 0.5, 0.5, 0.5, 1.0 });
		}
	};
}