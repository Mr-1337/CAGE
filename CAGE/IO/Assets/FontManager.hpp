#pragma once

#include "../../Graphics/Font/Font.hpp"
#include "AssetManager.hpp"
#include "SDL2/SDL_TTF.h"

namespace cage
{
	class FontManager : public AssetManager<Font>
	{
	public:
		TextureManager() : AssetManager("Fonts")
		{

		}
	protected:

		std::shared_ptr<Font> loadAssetFromFile(std::filesystem::path file) override
		{
			auto surface = IMG_Load(file.string().c_str());
			auto texture = std::make_shared<Texture>(surface);
			SDL_FreeSurface(surface);
			return texture;
		}

		std::filesystem::path getAssetSubdirectory() override
		{
			return "Fonts";
		}
	};
}