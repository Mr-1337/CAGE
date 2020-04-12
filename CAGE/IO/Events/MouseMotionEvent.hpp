#pragma once

namespace cage
{
	class MouseMotionEvent
	{
	public:

		// the final coordinates
		const int x, y;
		// the relative change in mouse position
		const int dx, dy;

		inline MouseMotionEvent(int finalX, int finalY, int deltaX, int deltaY) :
			x(finalX), y(finalY), dx(deltaX), dy(deltaY)
		{

		}

	};
}