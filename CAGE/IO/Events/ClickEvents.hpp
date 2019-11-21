#pragma once

#include "Event.hpp"

namespace cage
{
	class MouseClickEvent : public Event<MouseClickEvent>
	{
	public:

		// the coordinates of the click
		const int x, y;

		// was release (true), click (false)
		const bool wasRelease;

		enum class MouseButton : int
		{
			LEFT   = 0,
			RIGHT  = 1,
			MIDDLE = 2
		};

		// the button used to click
		const MouseButton button;

		inline MouseClickEvent(int x, int y, MouseButton button, bool release) :
			x(x), y(y), button(button), wasRelease(release)
		{

		}
	};
}