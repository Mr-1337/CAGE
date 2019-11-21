#include "Hoverable.hpp"

namespace cage
{
	namespace ui
	{
		class Clickable : public Hoverable
		{
		protected:
			virtual void onClick() = 0;
			virtual void onRelease() = 0;
		};
	}
}