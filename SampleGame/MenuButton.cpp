#include "MenuButton.hpp"

const cage::Font* MenuButton::s_font;

MenuButton::MenuButton(const std::string& text) :
	cage::ui::Button(
		std::make_shared<cage::Texture>(IMG_Load("Assets/button/idle.png")),
		std::make_shared<cage::Texture>(IMG_Load("Assets/button/hover.png")),
		std::make_shared<cage::Texture>(IMG_Load("Assets/button/pressed.png")))
{

	auto t = std::make_shared<cage::ui::Text>(*s_font);
	t->SetText(text);
	float w = t->GetSize().x;
	Resize(glm::max(GetSize(), { w + 50, 0 }));
	m_width = w;
	Add(t);
}