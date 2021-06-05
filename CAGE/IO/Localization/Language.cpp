#include "Language.hpp"

namespace cage
{
	Language::Language(const std::string& langCode) :
		k_gameAssetDirectory(std::filesystem::current_path() / "Assets" / "Localization" / langCode)
	{
		if (!std::filesystem::is_directory(k_gameAssetDirectory))
			throw std::runtime_error(std::string("A language directory doesn't exist for langCode " + langCode));
	}

	void Language::LoadFile(const std::filesystem::path& path)
	{
		m_stringTable.clear();
		if (m_currentFile.is_open())
			m_currentFile.close();
		m_currentFile.open(k_gameAssetDirectory / path);

		std::string key;
		while (std::getline(m_currentFile, key))
		{
			// comment, disregard
			if (key.find('#') == 0)
				continue;
			if (key.empty())
				continue;

			std::string value;
			std::getline(m_currentFile, value);

			m_stringTable.insert({ key, value });
			
		}

	}

	std::string Language::Get(const std::string& identifier)
	{
		return m_stringTable.at(identifier);
	}
}