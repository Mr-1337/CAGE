#pragma once

#include "../../Graphics/Textures/Texture.hpp"
#include "AssetManager.hpp"
#include "SDL2/SDL_image.h"

namespace cage
{
	class TextureManager : public AssetManager<Texture>
	{
	public:
		TextureManager() : AssetManager("Textures")
		{

		}
	protected:

		std::shared_ptr<Texture> loadAssetFromFile(std::filesystem::path file) override
		{
			auto surface = IMG_Load(file.string().c_str());
			auto texture = std::make_shared<Texture>(surface);
			SDL_FreeSurface(surface);
			return texture;
		}

		std::filesystem::path getAssetSubdirectory() override
		{
			return "Textures";
		}
	};
}