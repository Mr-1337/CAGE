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
		void Update(float dt);
	private:
		std::queue<ScriptAction*> m_actionQueue;
		World* m_world;
	};
}