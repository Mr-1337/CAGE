#pragma once

#include <fstream>
#include <string_view>
#include <format>
#include <iostream>

namespace cage
{
	class Game;
	namespace io
	{
		// All purpose logger for CAGE.
		class Logger final
		{
		public:
			enum class Severity
			{
				Info,
				Warning,
				Error
			};

			enum class Category
			{
				Misc,
				Graphics,
				Logic,
				IO,
				Audio,
				Threading,
				Network
			};

			static void Log(std::string_view message, Category category = Category::Misc, Severity severity = Severity::Info);
			static inline void LogWarn(std::string_view message, Category category = Category::Misc)
			{
				Log(message, category, Severity::Warning);
			}
			static inline void LogError(std::string_view message, Category category = Category::Misc)
			{
				Log(message, category, Severity::Error);
			}

			// Formatted log function, allows printf style logging utilizing std::format
			template<typename ...Types>
			static inline void Logf(Category category, Severity severity, const std::string& message, Types... args)
			{
				Log(std::vformat(message, std::make_format_args(args...)), category, severity);
			}

			template<typename ...Types>
			static inline void Logf(std::string_view message, Types... args)
			{
				Log(std::vformat(message, std::make_format_args(args...)));
			}
		private:
			friend class cage::Game;

			Logger();
			std::ofstream m_logFile;
			static Logger* s_instance;

			void log(std::string_view message, Category category, Severity severity);
		};
	}
}