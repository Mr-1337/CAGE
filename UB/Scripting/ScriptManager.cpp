#include "ScriptManager.hpp"
#include "StartAction.hpp"
#include "EntMoveAction.hpp"
#include "SoundAction.hpp"

namespace ub
{
	ScriptManager::ScriptManager()
	{
	
	}

	ScriptManager::~ScriptManager()
	{
		while (!m_actionQueue.empty())
		{
			delete m_actionQueue.front();
			m_actionQueue.pop();
		}
	}

	void ScriptManager::SetWorld(World* world)
	{
		m_world = world;
		Start();
	}

	void ScriptManager::Start()
	{
		m_actionQueue = std::queue<ScriptAction*>();
		m_actionQueue.push(new StartAction(m_world));

		m_actionQueue.push(new EntMoveAction(m_world, { 0.f, 0.f }, { 300.f, 300.f }, 1.0f));
		m_actionQueue.push(new EntMoveAction(m_world, { 300.f, 300.f }, { 0.f, 600.f }, 1.0f));
		m_actionQueue.push(new EntMoveAction(m_world, { 0.f, 600.f }, { -300.f, 300.f }, 1.0f));
		m_actionQueue.push(new EntMoveAction(m_world, { -300.f, 300.f }, { 0.f, 0.f }, 1.0f));
	}

	void ScriptManager::Enqueue(ScriptAction* action)
	{
		if (m_actionQueue.empty())
			m_actionQueue.push(new StartAction(m_world));
		m_actionQueue.push(action);
	}

	void ScriptManager::Update(float dt)
	{
		if (m_actionQueue.empty())
			return;
		m_actionQueue.front()->Update(dt);
		if (m_actionQueue.front()->IsComplete())
		{
			delete m_actionQueue.front();
			m_actionQueue.pop();
			if (!m_actionQueue.empty())
				m_actionQueue.front()->Initialize();
		}
	}
}