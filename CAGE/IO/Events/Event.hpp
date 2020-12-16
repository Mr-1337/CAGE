#pragma once

#include <string>
#include <variant>

#include "ClickEvents.hpp"
#include "KeyEvents.hpp"
#include "MouseMotionEvent.hpp"
#include "ScrollEvent.hpp"

namespace cage
{
	// A CAGE event. Application layer events system to remove dependence on OS specific event functionality
	using Event = std::variant<MouseClickEvent, MouseMotionEvent, KeyDownEvent, KeyUpEvent, TextEvent, ScrollEvent>;

}