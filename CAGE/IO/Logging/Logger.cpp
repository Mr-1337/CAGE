#include "Logger.hpp"
#include <iostream>
#include <chrono>
#include <format>

namespace cage
{
	namespace io
	{

		Logger* Logger::s_instance = nullptr;

		// Only Game class may instantiate
		Logger::Logger()
		{
			s_instance = this;
			m_logFile.open("CAGElog.txt", std::ios_base::trunc);
			Log("Logger online!", Category::IO);
		}

		void Logger::Log(std::string_view message, Logger::Category category, Logger::Severity severity)
		{
			s_instance->log(message, category, severity);
		}

		void Logger::log(std::string_view message, Logger::Category category, Logger::Severity severity)
		{
			auto const time = std::chrono::current_zone()->to_local(time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()));
			auto const timeStr = std::format("[{:%m/%d/%y %I:%M:%S %p}]", time);

			struct
			{
				uint8_t r, g, b;
			} 
			cols[] = 
			{
				{ 218, 218, 218 },
				{ 236, 164, 51 },
				{ 237, 33, 101 }
			};

			auto color = cols[static_cast<int>(severity)];
			std::string colStr = std::format("\x1b[38;2;{};{};{}m", color.r, color.g, color.b);

			const std::string categories[] =
			{
				"Misc",
				"Graphics",
				"Logic",
				"IO",
				"Audio",
				"Threading",
				"Network"
			};
			std::string catStr = "[" + categories[static_cast<int>(category)] + "] ";

			std::cout << colStr << timeStr << catStr << message << '\n';
			m_logFile << timeStr << catStr << message << '\n';
		}
	}
}