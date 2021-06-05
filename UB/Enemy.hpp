#pragma once
#include "Character.hpp"

namespace ub
{
	class Enemy : public Character
	{
	public:
		Enemy(World* world);

		void OnCollision(Entity* other) override;
		bool OnInteract() override;

	};
}