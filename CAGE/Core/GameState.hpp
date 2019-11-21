#pragma once

namespace cage
{
	class StateMachine;

	class GameState
	{
	public:
		virtual void ProcessEvents() = 0;
		virtual void Update(float delta) = 0;
		virtual void Draw() = 0;
		inline bool GetQuit() { return m_quit; }

		static StateMachine* s_stateMachine;

	protected:
		bool m_quit = false;
	};
}