#pragma once

#include "ScriptAction.hpp"

namespace ub
{
	class StartAction : public ScriptAction
	{
	public:
		StartAction(World* world);

		void Initialize() override;
		void Update(float dt) override;
	};
}