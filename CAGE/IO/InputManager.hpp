#include <SDL2/SDL.h>

#include "Events/EventDispatcher.hpp"

namespace cage
{

	class MouseState
	{
	public:

		enum Buttons : unsigned int
		{
			M1 = 1,
			M2 = 2,
			M3 = 4,
			M4 = 8,
			M5 = 16,
			M6 = 32,
			M7 = 64,
			M8 = 128
		};

		bool IsDown(Buttons button)
		{
			return ButtonMask & button;
		}

		unsigned int ButtonMask;

		int x;
		int y;

	};

	// The master object for all input. Facilitates both polling style input as well as several EventDispatchers for event driven input
	class InputManager
	{
	public:
		void Raise(SDL_Event& e);
		void Raise(Event& e);
		void Subscribe(EventListener* listener);
		void Unsubscribe(EventListener* listener);
		Event Convert(SDL_Event& e);
		MouseState GetMouseState();

	private:
		EventDispatcher m_dispatcher;
		unsigned int convertMask(Uint32 nativeMask);
	};
}