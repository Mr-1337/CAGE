#include "Player.hpp"
#include "World/World.hpp"

namespace ub
{
	Player::Player(World* world) : Character(world, "Player")
	{
		SetMaxHP(150);
		SetHP(GetMaxHP());
	}

	void Player::Serialize(std::ostream& os)
	{
		Character::Serialize(os);
		auto buffer = "Player Object";
		os.write(buffer, std::strlen(buffer));
	}

	void Player::ProcessInput(cage::Event& event)
	{
		if (auto ke = std::get_if<cage::KeyDownEvent>(&event))
		{
			if (ke->keyCode == SDLK_RETURN && !ke->repeat)
			{
				for (auto e : getWorld()->GetEntities())
				{
					if (e != this && colliding(e))
					{
						e->OnInteract();
					}
				}
			}
		}
	}

	void Player::OnCollision(Entity* other)
	{

	}
}