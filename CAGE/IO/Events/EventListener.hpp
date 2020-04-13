#pragma once

#include "Event.hpp"

namespace cage
{
	// A listener for CAGE events
	class EventListener
	{
	public:

		EventListener() : m_eventDepth(0)
		{

		}

		// Processes an event corresponding to this listener's type. Returns if the event in question is "finished" getting handled (should it further propogate to other listeners?)
		virtual bool HandleEvent(Event& event) = 0;

		inline void SetDepth(int d) { m_eventDepth = d; }
		inline int GetDepth() const { return m_eventDepth; }

	private:
		int m_eventDepth;
		
	};

}