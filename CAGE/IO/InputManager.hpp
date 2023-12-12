#pragma once

#include <SDL2/SDL.h>
#include <map>
#include <numeric>

#include "Events/EventDispatcher.hpp"
#include "Logging/Logger.hpp"
#include <GLM/glm/glm.hpp>

namespace cage
{
	namespace io
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

			int x, y, dx, dy;
		};

		// The master object for all input. Facilitates both polling style input as well as several EventDispatchers for event driven input
		class InputManager
		{
		public:
			InputManager();
			~InputManager();
			void Raise(SDL_Event& e);
			void Raise(Event&& e);
			void Subscribe(EventListener* listener);
			void Unsubscribe(EventListener* listener);
			Event Convert(SDL_Event& e);
			MouseState m_MouseState;

			// Represents a physical source of input. This is a data source that may map to a virtual control like "jump" or "forwards". Examples include controller stick axes, keyboard keys, mouse position, mouse buttons, HMD pose, etc.
			struct InputSrc
			{

				// What this input is
				enum class Type : unsigned int
				{
					Unbound,
					// Binary state buttons
					Key,
					ControllerButton,
					MouseButton,

					// Analog Axes
					KeyAxes,
					ControllerStickXY,
					ControllerStickXZ,
					ControllerStickYZ,
					ControllerDpad,
					ControllerTrigger,
					MouseDelta,
					MouseWheel
				} m_Type;

				// ID of buttons / sticks used for this control. These would map to the corresponding SDL enum values for the given type of input
				std::array<int, 6> m_ButtonIDs = { -1, -1, -1, -1, -1, -1 };

				bool operator==(const InputSrc& other) const
				{
					return m_ButtonIDs == other.m_ButtonIDs && m_Type == other.m_Type;
				}

				// TODO: Make factory methods to make binding easier. InputSrc MakeKey(keycode)

			};

			void RegisterControl(const std::string& control, InputSrc defaultBinding = InputSrc());
			void PollInput();

			std::map<std::string, glm::vec3>::const_iterator begin() const
			{
				return m_inputBuffer.cbegin();
			}

			std::map<std::string, glm::vec3>::const_iterator end() const
			{
				return m_inputBuffer.cend();
			}

			glm::vec3 operator[](const std::string& control) const
			{
				try
				{
					return m_inputBuffer.at(control);
				}
				catch (std::out_of_range& e)
				{
					Logger::LogError("Unknown control " + control, Logger::Category::IO);
					return glm::vec3{ 0.f, 0.f, 0.f };
				}
			}

		private:

			struct hash
			{
				std::size_t operator()(const InputManager::InputSrc& input) const
				{
					int sum = std::accumulate(input.m_ButtonIDs.begin(), input.m_ButtonIDs.end(), decltype(input.m_ButtonIDs)::value_type(0));
					return sum ^ static_cast<unsigned int>(input.m_Type);
				}
			};

			using InputBuffer = std::map<std::string, glm::vec3>;
			using ControlScheme = std::unordered_map<InputSrc, std::string, hash>;

			InputBuffer m_inputBuffer;
			ControlScheme m_controlScheme;
			EventDispatcher m_dispatcher;

			SDL_GameController* m_gameController;
			unsigned int convertMask(Uint32 nativeMask);
		};
	}
}