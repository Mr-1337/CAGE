#pragma once

#include <GLM/glm/glm.hpp>
#include <Core/Game.hpp>
#include <Graphics/Models/Mesh.hpp>
#include <Graphics/Vertex/Vertex.hpp>

namespace ub
{

	class World;

	class Entity
	{
		World* m_world;
		cage::Mesh<cage::Vertex3UV> m_mesh;
		bool m_collisionEnabled;
		bool m_interacting;
		glm::vec2 m_size;
		glm::vec2 m_position;
		const std::string m_name;
	protected:
		World* getWorld();

		bool colliding(Entity* other);

		bool interacting();
		void startInteraction();
		void endInteraction();

	public:

		static cage::Game* s_Game;

		Entity(World* world, const std::string& name);
		void LoadTexture(const std::string& name);

		virtual void Update(float dt);
		void Draw();

		void SetPosition(glm::vec2 pos);
		void SetSize(glm::vec2 size);

		inline glm::vec2 GetPosition() { return m_position; }
		inline glm::vec2 GetSize() { return m_size; }
		inline std::string GetName() { return m_name; }

		inline void EnableCollision() { m_collisionEnabled = true; }
		inline void DisableCollision() { m_collisionEnabled = false; }
		inline bool CollisionEnabled() { return m_collisionEnabled; }

		virtual bool OnInteract();
		virtual void OnCollision(Entity* other);

	private:
		void resolveCollision(Entity* other);
	};

}
