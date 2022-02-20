#include "World.hpp"
#include "../Scripting/ScriptManager.hpp"
#include "../Enemy.hpp"

#include <GLM/glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace ub
{
	World::World(std::pair<int, int> winSize) : 
		m_tilemap(16, 16),
		m_gridMesh("Grid Mesh"),
		m_cameraPos(0.f),
		m_zoom(1.0f),
		m_winSize(winSize),
		m_drawGrid(false),
		m_dirty(false),
		m_hackBuffer(6, cage::Vertex3UV(0, 0, 0, 0, 0)),
		m_battle(nullptr),
		m_control(true)
	{
		m_dialogue = std::make_shared<DialogueBox>();
		m_root.Add(m_dialogue);
		m_worldShader = std::make_shared<WorldShader>();

		m_player = new Player(this);
		m_player->LoadTexture("mr_placeholder.png");
		m_entities.push_back(m_player);

		m_dev = new Character(this, "Dev");
		m_dev->LoadTexture("Nuber.png");
		m_entities.push_back(m_dev);

		m_enemy = new Enemy(this);
		m_enemy->LoadTexture("Uber.png");
		m_entities.push_back(m_enemy);

		m_scriptManager = new ScriptManager();
		m_scriptManager->SetWorld(this);
		m_worldShader->Use();

		m_root.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);
		SetWinSize(winSize);

		//glClearColor(0.2, 0.0, 0.5, 0.0);
		//glClearColor(0.8, 0.5, 0.1, 1.0);

		GenWorld();
		Resize({ 30, 30 });

		house.x = 15;
		house.y = 10 - 2;
		std::shared_ptr<cage::Texture> tex = std::make_shared<cage::Texture>(IMG_Load("Assets/Textures/House2.png"));
		house.mesh = std::make_shared<cage::Mesh<cage::Vertex3UV>>("mesh");
		house.mesh->LoadTexture(tex);
		house.w = tex->GetSize().first;
		house.h = tex->GetSize().second;
		m_player->SetPosition({ 20.0, 20.0 });

	}

	void World::Zoom(int dy)
	{
		if (m_control)
		{
			if (dy > 0)
				m_zoom *= 1.1;
			else
				m_zoom /= 1.1;
		}
	}

	World::~World()
	{
		delete m_scriptManager;
	}
	
	glm::mat4 World::calcViewMatrix()
	{
		glm::vec2 camOffset = { m_winSize.first / 2, m_winSize.second / 2 };
		camOffset /= m_zoom;
		return glm::translate(glm::scale(glm::identity<glm::mat4>(), { m_zoom, m_zoom, 1.0 }), { (-m_cameraPos + camOffset), 0.f });
	}

	void World::Move(glm::vec2 position)
	{
		m_cameraPos += position;
		m_view = calcViewMatrix();
	}

	void World::Reload()
	{
		m_scriptManager->Start();
		m_cameraPos = { 0.f, 0.f };
		m_player->SetPosition({ 0.f, 0.f });
		m_pos2 = { 0.f, 0.f };
		m_zoom = 1.0f;

		m_view = calcViewMatrix();
	}

	void World::Resize(std::pair<int, int> worldSize)
	{
		m_tilemap = Tilemap(worldSize.first, worldSize.second);
		GenWorld();
	}

	void World::StartBattle(Enemy* enemy)
	{
		m_worldShader->Use();
		m_view = glm::identity<glm::mat4>();
		m_worldShader->m_ViewProjection->value = m_projection * m_view;
		m_worldShader->m_ViewProjection->ForwardToShader();
		m_battle = std::make_unique<Battle>(m_player, m_enemy, m_worldShader);
		m_battle->GetPanel()->Resize(m_root.GetSize());
		m_root.Add(m_battle->GetPanel());
	}

	std::vector<Entity*> World::GetEntities()
	{
		return m_entities;
	}

	glm::ivec2 World::ToTileIndices(int x, int y)
	{
		// convert from window coordinates into tiles indices
		glm::vec4 temp{ static_cast<float>(x), static_cast<float>(y), 0.f, 1.f };
		temp = (glm::inverse(m_view)) * temp;
		temp.x /= Tilemap::TILE_SIZE;
		temp.y /= Tilemap::TILE_SIZE;
		return { static_cast<int>(temp.x), static_cast<int>(temp.y) };
	}

	void World::PaintTileMouse(int mouseX, int mouseY, Tilemap::Tile type)
	{
		if (mouseX > m_winSize.first || mouseY > m_winSize.second)
			return;
		auto indices = ToTileIndices(mouseX, mouseY);
		PaintTileRaw(indices.x, indices.y, type);
	}

	void World::FillTileMouse(int mouseX, int mouseY, Tilemap::Tile type, int radius)
	{
		auto center = ToTileIndices(mouseX, mouseY);
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				auto tile = center + glm::ivec2{ x, y };
				PaintTileRaw(tile.x, tile.y, type);
			}
		}
	}

	void World::PaintTileRaw(int tileX, int tileY, Tilemap::Tile type)
	{
		if (tileX >= 0 && tileX < m_tilemap.GetSize().first && tileY >= 0 && tileY < m_tilemap.GetSize().second)
		{
			m_tilemap(tileX, tileY) = type;
			m_tilemap.UpdateMesh(tileX, tileY);
			m_dirty = true;
		}
	}

	void World::GenWorld()
	{	
		const int TILE_SIZE = Tilemap::TILE_SIZE;
		m_structureData.clear();
		for (int i = 0; i < 10; i++)
		{
			Structure fence;
			auto tex = std::make_shared<cage::Texture>(IMG_Load("Assets/Textures/fence.png"));
			fence.x = 4 + i;
			fence.y = 11;
			fence.w = tex->GetSize().first;
			fence.h = tex->GetSize().second;
			fence.mesh = std::make_shared<cage::Mesh<cage::Vertex3UV>>("mesh");
			fence.mesh->LoadTexture(tex);

			std::vector<cage::Vertex3UV> geometry;

			float fenceDepth = (float)(fence.y + (float)fence.h / TILE_SIZE) / m_tilemap.GetSize().second;

			const int FENCE_SIZE = 64;

			geometry.emplace_back(fence.x * FENCE_SIZE, fence.y * TILE_SIZE, fenceDepth, 0.f, 0.f);
			geometry.emplace_back(fence.x * FENCE_SIZE + fence.w, fence.y * TILE_SIZE, fenceDepth, 0.f + 1, 0.f);
			geometry.emplace_back(fence.x * FENCE_SIZE, fence.y * TILE_SIZE + fence.h, fenceDepth, 0.f, 1);

			geometry.emplace_back(fence.x * FENCE_SIZE + fence.w, fence.y * TILE_SIZE + 0.f, fenceDepth, 0.f + 1, 0.f);
			geometry.emplace_back(fence.x * FENCE_SIZE + 0.f, fence.y * TILE_SIZE + fence.h, fenceDepth, 0.f, 1);
			geometry.emplace_back(fence.x * FENCE_SIZE + fence.w, fence.y * TILE_SIZE + fence.h, fenceDepth, 1, 1);


			fence.mesh->GetBuffer().Fill(geometry);

			m_structureData.push_back(fence);
		}


		std::vector<cage::Vertex3UV> geometry;
		geometry.reserve(m_tilemap.GetSize().first * m_tilemap.GetSize().second * 6);

		auto makeLine = [&geometry, this, TILE_SIZE](int x1, int x2, int y1, int y2) -> void
		{
			geometry.emplace_back(static_cast<float>(TILE_SIZE) * glm::vec3{ x1, y1, 0.01 }, glm::vec2{ 0.f, 0.f });
			geometry.emplace_back(static_cast<float>(TILE_SIZE) * glm::vec3{ x2, y2, 0.01 }, glm::vec2{ 0.f, 0.f });
		};

		auto worldSize = m_tilemap.GetSize();
		
		for (int y = 0; y <= worldSize.second; y++)
		{
			makeLine(0, worldSize.first , y , y);
		}
		for (int x = 0; x <= worldSize.first; x++)
		{
			makeLine(x, x, 0, worldSize.second);
		}
		m_gridMesh.GetBuffer().Fill(geometry);

	}

	void World::SetWinSize(std::pair<int, int> winSize)
	{
		m_winSize = winSize;
		m_projection = glm::ortho(0.f, (float)winSize.first, (float)winSize.second, 0.f);
		m_view = calcViewMatrix();

		cage::ui::UIElement::shader->Use();
		cage::ui::UIElement::shader->Projection->value = m_projection;
		cage::ui::UIElement::shader->Projection->ForwardToShader();

		m_root.Resize({ winSize.first, winSize.second });
		if (InBattle())
			m_battle->GetPanel()->Resize({ winSize.first, winSize.second });
	}

	void World::MoveEnt(unsigned int id, glm::vec2 position)
	{
		switch (id)
		{
		case 0:
			
			break;
		case 1:
			m_pos2 = position;
			break;
		}
	}

	void World::Update(float dt)
	{
		if (InBattle())
		{
			m_battle->Update(dt);
			if (m_battle->Complete())
			{
				m_root.Remove(m_battle->GetPanel());
				m_battle = nullptr;
			}
		}
		else
		{
			float speed = 256.f;
			m_velocity = { 0.0, 0.0 };

			auto keys = SDL_GetKeyboardState(NULL);
			if (m_control)
			{
				if (keys[SDL_SCANCODE_W])
					m_velocity.y = -speed;
				if (keys[SDL_SCANCODE_A])
					m_velocity.x = -speed;
				if (keys[SDL_SCANCODE_S])
					m_velocity.y = speed;
				if (keys[SDL_SCANCODE_D])
					m_velocity.x = speed;

				const glm::vec2 playerOffset = glm::vec2(28.f, 48.f) * 0.5f;
				const float moveTime = 1.0f / 0.2f;
				m_cameraPos += (m_player->GetPosition() + playerOffset - m_cameraPos) * moveTime * dt;

				auto pos = m_player->GetPosition() + m_velocity * dt;
				m_player->SetPosition(pos);
				m_dev->SetPosition(m_pos2);

				m_view = calcViewMatrix();

				for (auto e : m_entities)
					e->Update(dt);

				m_scriptManager->Update(dt);
			}

		}

		m_root.Update(dt);
	}

	void World::Draw()
	{
		const int TILE_SIZE = Tilemap::TILE_SIZE;
		
		glEnable(GL_DEPTH_TEST);
		m_worldShader->Use();
		std::vector<cage::Vertex3UV> geometry;
		geometry.reserve(6);

		float houseDepth = (float)(house.y + (float)house.h / TILE_SIZE) / m_tilemap.GetSize().second;

		geometry.emplace_back(house.x * TILE_SIZE, house.y * TILE_SIZE, houseDepth, 0.f, 0.f);
		geometry.emplace_back(house.x * TILE_SIZE + house.w, house.y * TILE_SIZE, houseDepth, 0.f + 1, 0.f);
		geometry.emplace_back(house.x * TILE_SIZE, house.y * TILE_SIZE + house.h, houseDepth, 0.f, 1);

		geometry.emplace_back(house.x * TILE_SIZE + house.w, house.y * TILE_SIZE + 0.f, houseDepth, 0.f + 1, 0.f);
		geometry.emplace_back(house.x * TILE_SIZE + 0.f, house.y * TILE_SIZE + house.h, houseDepth, 0.f, 1);
		geometry.emplace_back(house.x * TILE_SIZE + house.w, house.y * TILE_SIZE + house.h, houseDepth, 1, 1);

		house.mesh->GetBuffer().Fill(geometry);

		m_worldShader->m_ViewProjection->value = m_projection * m_view;
		m_worldShader->m_ViewProjection->ForwardToShader();

		m_worldShader->m_Textured->value = true;
		m_worldShader->m_Textured->ForwardToShader();
		m_worldShader->m_Color->ForwardToShader();

		m_tilemap.Draw();
		house.mesh->Draw();

		for (auto& s : m_structureData)
		{
			auto mat = glm::identity<glm::mat4>();
			//mat[1][0] = -m_cameraShear.x;
			m_worldShader->m_Model->value = mat;
			m_worldShader->m_Model->ForwardToShader();
			m_worldShader->m_ViewProjection->value = m_projection * m_view;
			m_worldShader->m_ViewProjection->ForwardToShader();
			s.mesh->Draw();
		}

		for (auto e : m_entities)
		{
			e->Draw();
		}

		m_worldShader->m_Model->value = glm::identity<glm::mat4>();
		m_worldShader->m_Model->ForwardToShader();

		if (m_drawGrid)
		{
			m_worldShader->m_Textured->value = false;
			m_worldShader->m_Color->value = { 1.0, 1.0, 1.0, 0.3 };

			m_worldShader->m_Textured->ForwardToShader();
			m_worldShader->m_Color->ForwardToShader();

			m_gridMesh.Draw(GL_LINES);
		}

		if (InBattle())
		{
			m_battle->Draw();
		}
		

		// Draw the UI no matter what
		glDisable(GL_DEPTH_TEST);
		cage::ui::UIElement::shader->Use();
		m_root.Draw();
	}

	void World::HandleEvents(cage::Event&& e)
	{
		if (auto se = std::get_if<cage::ScrollEvent>(&e))
			Zoom(se->amount);
		else if (auto we = std::get_if<cage::WindowEvent>(&e))
		{
			SetWinSize({ we->data1, we->data2 });
		}
		m_player->ProcessInput(e);
		if (InBattle())
			m_battle->HandleEvents(e);
	}
}