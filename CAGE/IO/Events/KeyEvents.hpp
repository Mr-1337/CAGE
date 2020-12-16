#pragma once

#include <string>

namespace cage
{
	class KeyDownEvent
	{
	public:
		const int keyCode;
		KeyDownEvent(int keyCode) : keyCode(keyCode)
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