#pragma once

#include <vector>
#include <algorithm>
#include "EventListener.hpp"

namespace cage
{
	// The EventDispatcher maintains lists of all objects that need to be notified of an event 
	class EventDispatcher
	{
	public:

		// Subscribes a listener to this dispatcher
		void Subscribe(EventListener* listener)
		{
			m_listeners.push_back(listener);

			std::sort(m_listeners.begin(), m_listeners.end(), [](auto l1, auto l2) { return l1->GetDepth() > l2->GetDepth();  });
		}
		
		// Removes a listener
		void Unsubscribe(EventListener* listener)
		{
			m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener));
		}

		// Notify will send the event to all listeners interested in EventType
		void Notify(Event e)
		{
			for (auto l : m_listeners)
			{
				// Have each listener handle the event. If the event is marked as handled, return
				if (l->HandleEvent(e))
					return;
			}
		}
	private:
		std::vector<EventListener*> m_listeners;

	};
}