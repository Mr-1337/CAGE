#pragma once

#include<SDL2/SDL_image.h>

#include "../CAGE/Graphics/UI/Button.hpp"
#include "../CAGE/Graphics/UI/Text.hpp"

class MenuButton : public cage::ui::Button
{
	float m_width;
public:
	
	static const cage::Font* s_font; 

	MenuButton(const std::string& text);

	virtual inline void SetActiveTexture(std::shared_ptr<cage::Texture> texture) override
	{
		if (texture)
		{
			setTextured(true);
			m_currentTexture = texture;
			//Resize({ texture->GetSize().first, texture->GetSize().second });
		}
	}

};