#include "Entity.hpp"
#include "World/World.hpp"

namespace ub
{

	cage::Game* Entity::s_Game = nullptr;

	Entity::Entity(World* world, const std::string& name) : m_world(world), m_mesh("Entity"), m_collisionEnabled(true), m_interacting(false), m_name(name)
	{
		m_size = { 16.0, 16.0 };
		m_position = { world->GetSize().first / 2, world->GetSize().second / 2 };
		m_position *= Tilemap::TILE_SIZE;
	}

	bool Entity::interacting()
	{
		return m_interacting;
	}

	void Entity::LoadTexture(const std::string& name)
	{
		auto tex = s_Game->GetTextureManager().Get(name);
		m_mesh.LoadTexture(tex);
		auto size = tex->GetSize();
		SetSize({ size.first, size.second });
	}

	World* Entity::getWorld()
	{
		return m_world;
	}

	void Entity::startInteraction()
	{
		m_interacting = true;
		m_world->SetControl(false);
	}

	void Entity::endInteraction()
	{
		m_interacting = false;
		m_world->SetControl(true);
	}

	void Entity::SetPosition(glm::vec2 pos)
	{
		m_position = pos;
		if (m_collisionEnabled)
		{
			for (auto e : m_world->GetEntities())
			{
				if (e == this)
					continue;
				else if (colliding(e) && e->CollisionEnabled())
					e->OnCollision(this);
			}
		}
	}

	void Entity::SetSize(glm::vec2 size)
	{
		m_size = size;
	}

	void Entity::Update(float dt)
	{
		auto ents = m_world->GetEntities();

		for (auto e : ents)
		{

			if (e == this)
				continue;
			else if (colliding(e))
				OnCollision(e);
		}
	}

	void Entity::Draw()
	{
		std::vector<cage::Vertex3UV> geometry;
		geometry.reserve(6);

		float depth = (m_position.y + m_size.y) / (m_world->GetSize().second * Tilemap::TILE_SIZE);

		depth = glm::min(depth, 1.0f);

		geometry.emplace_back(m_position.x + 0.f, m_position.y + 0.f, depth, 0.f, 0.f);
		geometry.emplace_back(m_position.x + m_size.x, m_position.y + 0.f, depth, 0.f + 1, 0.f);
		geometry.emplace_back(m_position.x + 0.f, m_position.y + m_size.y, depth, 0.f, 1);

		geometry.emplace_back(m_position.x + m_size.x, m_position.y + 0.f, depth, 0.f + 1, 0.f);
		geometry.emplace_back(m_position.x + 0.f, m_position.y + m_size.y, depth, 0.f, 1);
		geometry.emplace_back(m_position.x + m_size.x, m_position.y + m_size.y, depth, 1, 1);

		m_mesh.SetGeometry(geometry);

		m_mesh.Draw();
	}

	bool Entity::colliding(Entity* other)
	{
		return !(m_position.x > other->m_position.x + other->m_size.x || m_position.x + m_size.x < other->m_position.x || m_position.y > other->m_position.y + other->m_size.y || m_position.y + m_size.y < other->m_position.y);
	}

	bool Entity::OnInteract()
	{
		return true;
	}

	void Entity::OnCollision(Entity* other)
	{
		resolveCollision(other);
	}

	void Entity::resolveCollision(Entity* other)
	{
		
	}
}