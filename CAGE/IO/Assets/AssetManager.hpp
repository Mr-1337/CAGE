#pragma once

#include <unordered_map>
#include <memory>
#include <filesystem>
#include <iostream>


namespace cage
{
	// CAGE assets are loaded once from disk by the asset manager for use by the application. Resources are stored in a hashmap that relates the filename to the resource in question.
	// Any managed asset that needs to be loaded only once and then automatically get freed is maintained by this system, and it removes the need for client code to load files (hopefully)
	template <typename AssetType>
	class AssetManager
	{
		std::unordered_map<std::string, std::shared_ptr<AssetType>> m_assetMap;

		const std::filesystem::path k_gameAssetDirectory;
		const std::string k_name;

	public:
		
		AssetManager(const std::string& name) : k_gameAssetDirectory(std::filesystem::current_path() / "Assets"), k_name(name)
		{

		}

		std::shared_ptr<AssetType> Get(const std::string& name)
		{
			if (m_assetMap.find(name) == m_assetMap.end())
			{
				std::cout << k_name << " asset map did not contain resource " << name << ", attempting to load from disk" << std::endl;
				m_assetMap[name] = loadAssetFromFile(k_gameAssetDirectory / getAssetSubdirectory() / name);
			}
			return m_assetMap[name];
		}
	
	protected:
		// Loads the specified asset from disk
		virtual std::shared_ptr<AssetType> loadAssetFromFile(std::filesystem::path file) = 0;
		virtual std::filesystem::path getAssetSubdirectory() = 0;
		
	};
}