#pragma once

#include "AssetManager.hpp"
#include "../../Audio/Sound.hpp"

namespace cage
{
	class SoundManager : public AssetManager<audio::Sound>
	{
	public:
		SoundManager() : AssetManager("Sounds")
		{

		}
	protected:
		std::shared_ptr<audio::Sound> loadAssetFromFile(std::filesystem::path file) override
		{
			return std::make_shared<audio::Sound>(file.string().c_str());
		}

		std::filesystem::path getAssetSubdirectory() override
		{
			return "Sounds";
		}
	};
}