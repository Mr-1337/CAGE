#include "Entity.hpp"

namespace v2
{
    Entity::Entity(cage::TextureManager& tm, const std::string& name) : m_mesh(name), x(0.f), y(0.f), m_startFrame(0), Added(false), Dead(false)
    {
        auto tex = tm.Get(name + ".png");
        m_mesh.LoadTexture(tex);

		w = tex->GetSize().first / 4;
		h = tex->GetSize().second / 4;
    }

	void Entity::Init(float x, float y, int startFrame)
	{
		this->x = x;
		this->y = y;
		this->m_startFrame = startFrame;
	}

	void Entity::Draw()
	{
		std::vector<cage::Vertex2UV> geometry;

		geometry.emplace_back(cage::Vertex2UV{ x + 0.f, y + 0.f, 0.f, 0.f });
		geometry.emplace_back(cage::Vertex2UV{ x + w,   y + 0.f, 1.f, 0.f });
		geometry.emplace_back(cage::Vertex2UV{ x + 0.f, y + h, 0.f, 1.f });
		geometry.emplace_back(cage::Vertex2UV{ x + w,   y + 0.f, 1.f, 0.f });
		geometry.emplace_back(cage::Vertex2UV{ x + 0.f, y + h, 0.f, 1.f });
		geometry.emplace_back(cage::Vertex2UV{ x + w,   y + h, 1.f, 1.f });

		m_mesh.SetGeometry(geometry);
		m_mesh.Draw();
	}

	int Entity::GetFrame()
	{
		return m_startFrame;
	}

	Entity::~Entity()
	{

	}
}