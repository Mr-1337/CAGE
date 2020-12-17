#pragma once

#include "Endpoint.hpp"

namespace cage
{
	namespace networking
	{
		class Connection
		{
			float m_timeout;
			const float k_timeoutThreshold;
		public:

			Connection() : m_timeout(0), k_timeoutThreshold(10.f)
			{

			}

			virtual void Receive(void* dataBuffer, size_t size) = 0;
			virtual void Send(void* dataBuffer, size_t size) = 0;

			// Returns true if the timer advanced and the connection is still active. Returns false if the connection appears to have timed out.
			bool AdvanceTimeout(float dt)
			{
				m_timeout += dt;
				return m_timeout < k_timeoutThreshold;
			}
		};
	}
}