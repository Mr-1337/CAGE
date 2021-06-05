#pragma once

#include "Character.hpp"

namespace ub
{

	class Player : public Character
	{
	public:
		Player(World* world);
		void ProcessInput(cage::Event& event);
		void OnCollision(Entity* other) override;
	};

}
