#pragma once

#include <string>

namespace cage
{
	class KeyDownEvent
	{
	public:
		const int keyCode;
		bool repeat;
		KeyDownEvent(int keyCode, bool repeat) : keyCode(keyCode), repeat(repeat)
		{

		}
	};

	class KeyUpEvent
	{
	};

	class TextEvent
	{
	public:
		const std::string text;
		TextEvent(const std::string& text) : text(text)
		{

		}
	};
}