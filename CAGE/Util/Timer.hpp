#pragma once

#include <chrono>
#include <functional>

namespace cage
{
	class Timer
	{
	public:
		Timer(std::chrono::duration<float> durationSecs, std::function<void(void)> callback);
		void Start();
		void Update();
		~Timer();
	private:
		std::function<void(void)> m_callback;
		std::chrono::duration<float> m_durationSecs;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
	};
}