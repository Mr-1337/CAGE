#pragma once

#include<SDL2/SDL_image.h>

#include "../../CAGE/Graphics/UI/Text.hpp"
#include "../../CAGE/Graphics/UI/Button.hpp"

class MenuButton : public cage::ui::Button
{
public:
	
	//inline static TTF_Font* s_font = TTF_OpenFont("Assets/sans.ttf", 72);

	MenuButton(const std::string& text) : 
		cage::ui::Button(
			std::make_shared<cage::Texture>(IMG_Load("Assets/button/idle.png")), 
			std::make_shared<cage::Texture>(IMG_Load("Assets/button/hover.png")), 
			std::make_shared<cage::Texture>(IMG_Load("Assets/button/pressed.png")))
	{
		auto s_font = TTF_OpenFont("Assets/sans.ttf", 36);
		auto t = std::make_shared<cage::ui::Text>(s_font);
		t->SetText(text);
		Add(t);
	}
};