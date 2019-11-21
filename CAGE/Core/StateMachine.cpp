#include "StateMachine.hpp"

namespace cage
{

	StateMachine* GameState::s_stateMachine;

	StateMachine::StateMachine()
	{
		GameState::s_stateMachine = this;
	}

	void StateMachine::ProcessEvents()
	{
		m_stateStack.top()->ProcessEvents();
	}

	void StateMachine::Update(float delta)
	{
		m_stateStack.top()->Update(delta);
		m_quit = m_stateStack.top()->GetQuit();
	}

	void StateMachine::Draw()
	{
		m_stateStack.top()->Draw();
	}

	void StateMachine::Push(GameState* state)
	{
		m_stateStack.push(state);
	}

	bool StateMachine::Quit()
	{
		return m_quit;
	}
}