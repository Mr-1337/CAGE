#pragma once

#include <list>

#include "EventListener.hpp"

namespace cage
{

	template <class EventType>
	class Event
	{
	protected:
		inline static std::list<EventListener<EventType>* > s_subscribers;
	public:

		inline static void Subscribe(EventListener<EventType>* listener)
		{
			s_subscribers.push_back(listener);
		}

		inline static void Unsubscribe(EventListener<EventType>* listener)
		{
			s_subscribers.remove(listener);
		}

		// Sends this event to all subscribers of the event. If the event is marked as handled, the loop will break
		inline static void RaiseEvent(EventType event)
		{
			for (auto& s : s_subscribers)
			{
				if (s->HandleEvent(event))
					break;
			}
		}

	};

}