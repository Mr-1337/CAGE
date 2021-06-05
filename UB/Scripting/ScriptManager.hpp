#pragma once
#include <queue>

#include "ScriptAction.hpp"
#include "../World.hpp"

namespace ub
{
	class ScriptManager
	{
	public:
		ScriptManager();
		~ScriptManager();

		void SetWorld(World* world);
		void Start();
		void Enqueue(ScriptAction* action);
		void Update(float dt);

		inline bool Empty() { return m_actionQueue.empty(); }

	private:
		std::queue<ScriptAction*> m_actionQueue;
		World* m_world;
	};
}