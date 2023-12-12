#include "InputManager.hpp"

namespace cage
{
	namespace io
	{
		InputManager::InputManager() : m_gameController(nullptr)
		{
			// open and grab handle to first controller we detect plugged in, no multi controller for now :(
			if (SDL_NumJoysticks() > 0)
			{
				m_gameController = SDL_GameControllerOpen(0);
				Logger::Logf(Logger::Category::IO, Logger::Severity::Info, "Controller detected! Opening controller: {}", SDL_GameControllerName(m_gameController));
			}
			else
			{
				Logger::Log("No controller detected", Logger::Category::IO, Logger::Severity::Info);
			}
		}

		InputManager::~InputManager()
		{
			if (m_gameController)
			{
				SDL_GameControllerClose(m_gameController);
			}
		}

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

		void InputManager::RegisterControl(const std::string& control, InputSrc defaultBinding)
		{
			m_controlScheme.insert({ defaultBinding, control });
			m_inputBuffer.insert({ control, glm::vec3() });
		}

		void InputManager::PollInput()
		{
			// Poll raw inputs first

			// Mouse
			// bitmask = SDL_GetMouseState(&m_MouseState.x, &m_MouseState.y);
			SDL_GetRelativeMouseState(&m_MouseState.dx, &m_MouseState.dy);
			//bitmask = convertMask(bitmask);
			//m_MouseState.ButtonMask = bitmask;

			// Keyboard
			auto keyState = SDL_GetKeyboardState(nullptr);

			struct ControllerState
			{
				glm::vec2 sticks[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
				glm::vec2& leftStick = sticks[0];
				glm::vec2& rightStick = sticks[1];
			} controllerState;

			// Controller
			if (m_gameController)
			{
				controllerState.leftStick.x = SDL_GameControllerGetAxis(m_gameController, SDL_CONTROLLER_AXIS_LEFTX) / 32768.f;
				controllerState.leftStick.y = SDL_GameControllerGetAxis(m_gameController, SDL_CONTROLLER_AXIS_LEFTY) / 32768.f;
				controllerState.rightStick.x = SDL_GameControllerGetAxis(m_gameController, SDL_CONTROLLER_AXIS_RIGHTX) / 32768.f;
				controllerState.rightStick.y = SDL_GameControllerGetAxis(m_gameController, SDL_CONTROLLER_AXIS_RIGHTY) / 32768.f;
			}

			// VR

			// Set virtual control values
			for (auto& control : m_controlScheme)
			{
				switch (control.first.m_Type)
				{
				case InputSrc::Type::Key:
					m_inputBuffer[control.second] = glm::vec3{ 1.0f * (keyState[control.first.m_ButtonIDs[0]]), 0.0f, 0.0f };
					break;

				case InputSrc::Type::KeyAxes:
				{
					glm::vec3 output = { 0, 0, 0 };
					glm::vec3 axes[6] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
					for (int i = 0; i < 6; i++)
					{
						if (control.first.m_ButtonIDs[i] == -1)
							continue;
						output += axes[i] * static_cast<float>(keyState[control.first.m_ButtonIDs[i]]);
					}
					m_inputBuffer[control.second] = output;
				}
				break;
				case InputSrc::Type::MouseDelta:
				{
					m_inputBuffer[control.second] = glm::vec3{ static_cast<float>(m_MouseState.dx), static_cast<float>(m_MouseState.dy), 0.0f };
				}
				break;
				case InputSrc::Type::ControllerStickXY:
				{
					auto stick = controllerState.sticks[control.first.m_ButtonIDs[0]];
					m_inputBuffer[control.second] = glm::vec3{ stick.x, stick.y, 0.0f };
				}
				break;
				case InputSrc::Type::ControllerStickXZ:
				{
					auto stick = controllerState.sticks[control.first.m_ButtonIDs[0]];
					m_inputBuffer[control.second] = glm::vec3{ stick.x, 0.0f, stick.y };
				}
				break;
				case InputSrc::Type::ControllerStickYZ:
				{
					auto stick = controllerState.sticks[control.first.m_ButtonIDs[0]];
					m_inputBuffer[control.second] = glm::vec3{ 0.0f, stick.y, stick.x };
				}
				break;
				}
			}
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
}