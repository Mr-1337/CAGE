#pragma once

namespace cage
{

	template <class EventType>
	class Event;

	template <class EventType>
	class EventListener
	{
	public:

		// Construction will auto subscribe a listener to its designated event type
		EventListener()
		{
			EventType::Subscribe(this);
		}

		// Processes an event corresponding to this listener's type. Returns if the event in question is "finished" getting handled (should it further propogate to other listeners?)
		virtual bool HandleEvent(EventType event) = 0;

		// Destruction will unsubscribe an event listener
		~EventListener()
		{
			EventType::Unsubscribe(this);
		}
		
	};

}