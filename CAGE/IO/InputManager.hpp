#include <SDL2/SDL.h>

#include "Events/EventDispatcher.hpp"

namespace cage
{
	// The master object for all input. Facilitates both polling style input as well as several EventDispatchers for event driven input
	class InputManager
	{
	public:
		void Raise(SDL_Event& e);
		void Subscribe(EventListener* listener);
	private:
		EventDispatcher m_dispatcher;
	};
}