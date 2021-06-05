#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace cage
{
	class Language
	{
		std::ifstream m_currentFile;
		std::filesystem::path k_gameAssetDirectory;

		std::unordered_map<std::string, std::string> m_stringTable;

	public:
		Language(const std::string& langCode);
		void LoadFile(const std::filesystem::path& path);
		std::string Get(const std::string& identifier);
	};
}