#include "InputManager.hpp"

namespace cage
{

	void InputManager::Raise(SDL_Event& e)
	{
		Raise(Convert(e));
	}

	void InputManager::Raise(Event&& e)
	{
		m_dispatcher.Notify(e);
	}

	Event InputManager::Convert(SDL_Event& e)
	{
		Event event = KeyDownEvent(65, false);

		// click, motion, down, up, wheel
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

			event.emplace<MouseClickEvent>(e.button.x, e.button.y, button, true);
			break;
		}
		case SDL_MOUSEMOTION:
		{
			event.emplace<MouseMotionEvent>(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			event.emplace<ScrollEvent>(e.wheel.y);
			break;
		}
		case SDL_TEXTINPUT:
		{
			event.emplace<TextEvent>(e.text.text);
			break;
		}
		case SDL_KEYDOWN:
			event.emplace<KeyDownEvent>(e.key.keysym.sym, e.key.repeat);
			break;
		case SDL_KEYUP:
			break;
		case SDL_WINDOWEVENT:
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				event.emplace<WindowEvent>(WindowEvent::Type::RESIZE, e.window.data1, e.window.data2);
			}
			break;
		}
		}

		return event;
	}

	MouseState InputManager::GetMouseState()
	{
		MouseState state;
		auto bitmask = SDL_GetMouseState(&state.x, &state.y);
		bitmask = convertMask(bitmask);
		state.ButtonMask = bitmask;
		return state;
	}

	unsigned int InputManager::convertMask(Uint32 nativeMask)
	{
		unsigned int mask = 0;
		if (nativeMask & SDL_BUTTON_LMASK)
			mask |= MouseState::M1;
		if (nativeMask & SDL_BUTTON_RMASK)
			mask |= MouseState::M2;
		if (nativeMask & SDL_BUTTON_MMASK)
			mask |= MouseState::M3;
		if (nativeMask & SDL_BUTTON(4))
			mask |= MouseState::M4;
		if (nativeMask & SDL_BUTTON(5))
			mask |= MouseState::M5;
		if (nativeMask & SDL_BUTTON(6))
			mask |= MouseState::M6;
		if (nativeMask & SDL_BUTTON(7))
			mask |= MouseState::M7;
		if (nativeMask & SDL_BUTTON(8))
			mask |= MouseState::M8;
		return mask;
	}

	void InputManager::Subscribe(EventListener* listener)
	{
		m_dispatcher.Subscribe(listener);
	}

	void InputManager::Unsubscribe(EventListener* listener)
	{
		m_dispatcher.Unsubscribe(listener);
	}
}