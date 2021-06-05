#include "Enemy.hpp"
#include "World.hpp"
#include "Scripting/ScriptManager.hpp"
#include "Scripting/StartBattleAction.hpp"

namespace ub
{
	Enemy::Enemy(World* world) : Character(world, "Enemy")
	{

	}

	void Enemy::OnCollision(Entity* other)
	{
		if (other == getWorld()->GetPlayer())
		{
			// start battle
		}
	}

	bool Enemy::OnInteract()
	{
		if (!interacting())
		{
			startInteraction();
			//getWorld()->GetPlayer()->Say("So clearly you have chosen death", "evil");
		}
		else
		{
			endInteraction();
			getWorld()->GetScriptManager()->Enqueue(new StartBattleAction(getWorld(), this));
		}
		return true;
	}
}