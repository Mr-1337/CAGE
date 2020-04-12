#include "InputManager.hpp"

namespace cage
{
	void InputManager::Raise(SDL_Event& e)
	{
		
		Event event = KeyDownEvent();

		// click, motion, down, up
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
		{
			Uint8 sdlButton = e.button.button;
			MouseClickEvent::MouseButton button = (MouseClickEvent::MouseButton)sdlButton;

			event.emplace<MouseClickEvent>(MouseClickEvent(e.button.x, e.button.y, button, false));
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			Uint8 sdlButton = e.button.button;
			MouseClickEvent::MouseButton button = (MouseClickEvent::MouseButton)sdlButton;

			event.emplace<MouseClickEvent>(MouseClickEvent(e.button.x, e.button.y, button, true));
			break;
		}
		case SDL_MOUSEMOTION:
		{
			event.emplace<MouseMotionEvent>(MouseMotionEvent(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel));
			break;
		}
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		default:
			return;
		}

		m_dispatcher.Notify(event);
	}

	void InputManager::Subscribe(EventListener* listener)
	{
		m_dispatcher.Subscribe(listener);
	}
}