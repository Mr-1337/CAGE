#pragma once

namespace cage
{
	class ScrollEvent
	{
	public:

		// the amount of scroll
		const int amount;

		inline ScrollEvent(int dy) :
			amount(dy)
		{

		}

	};
}