#include "Timer.hpp"

namespace cage
{
	Timer::Timer(std::chrono::duration<float> durationSecs, std::function<void(void)> callback) : m_durationSecs(durationSecs), m_callback(callback)
	{

	}

	void Timer::Start()
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update()
	{
		auto now = std::chrono::high_resolution_clock::now();
		if (now - m_startTime >= m_durationSecs)
		{
			m_startTime = now;
			m_callback();
		}
	}

	Timer::~Timer()
	{

	}
}