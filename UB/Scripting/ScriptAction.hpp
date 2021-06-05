#pragma once

#include "../World.hpp"

namespace ub
{
	class ScriptAction
	{
	public:
		ScriptAction(World* world);
		virtual ~ScriptAction();
		virtual void Initialize() = 0;
		virtual void Update(float dt) = 0;
		inline bool IsComplete() { return m_complete; };
	protected:
		bool m_complete;
		World* m_world;
	};
}