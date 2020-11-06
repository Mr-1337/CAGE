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

	void StateMachine::Update(float dt)
	{
		m_stateStack.top()->Update(dt);
		m_quit = m_stateStack.top()->GetQuit();
	}

	void StateMachine::Draw()
	{
		m_stateStack.top()->Draw();
	}

	void StateMachine::Push(GameState* state)
	{
		m_stateStack.push(state);
		m_stateStack.top()->OnRevealed();
	}

	void StateMachine::Pop()
	{
		m_stateStack.pop();
		if (!m_stateStack.empty())
			m_stateStack.top()->OnRevealed();
	}

	bool StateMachine::Quit()
	{
		if (m_quit)
		{
			delete m_stateStack.top();
			Pop();
		}
		return m_stateStack.empty();
	}
}