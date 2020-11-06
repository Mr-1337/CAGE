#include "World.hpp"
#include "Scripting/ScriptManager.hpp"

#include <GLM/glm/gtc/matrix_transform.hpp>
#include <exception>

namespace ub
{
	World::World(std::pair<int, int> winSize) : 
		m_worldMesh("World Mesh", true),
		m_playerMesh("Player Mesh"),
		m_uberMesh("Uber Mesh"),
		m_gridMesh("Grid Mesh"),
		m_cameraPos(0.f),
		m_position(0.f),
		m_zoom(1.0f),
		m_winSize(winSize),
		m_worldSize(16, 16),
		m_drawGrid(false),
		m_dirty(false),
		m_hackBuffer(6, cage::Vertex2UV(0, 0, 0, 0))
	{

		m_worldShader = std::make_shared<WorldShader>();

		auto surface = IMG_Load("Assets/Textures/UBatlas.png");
		m_worldMesh.LoadTexture(surface);
		SDL_FreeSurface(surface);

		m_playerMesh.LoadTexture(surface = IMG_Load("Assets/Textures/mr_placeholder.png"));
		SDL_FreeSurface(surface);
		m_uberMesh.LoadTexture(surface = IMG_Load("Assets/Textures/Uber.png"));
		SDL_FreeSurface(surface);

		m_scriptManager = new ScriptManager();
		m_scriptManager->SetWorld(this);
		m_worldShader->Use();



		SetWinSize(winSize);

		//glClearColor(0.2, 0.0, 0.5, 0.0);
		//glClearColor(0.8, 0.5, 0.1, 1.0);

		GenWorld();
		Resize(m_worldSize);
	}

	void World::Zoom(int dy)
	{
		m_zoom *= 1.0 + 0.1 * (dy > 0) - 0.1 * (dy < 0);
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
		m_position = { 0.f, 0.f };
		m_pos2 = { 0.f, 0.f };
		m_zoom = 1.0f;

		m_view = calcViewMatrix();
	}

	void World::Resize(std::pair<int, int> worldSize)
	{
		m_worldSize = worldSize;
		m_tileData.clear();
		m_tileData.resize(m_worldSize.first * m_worldSize.second);
		GenWorld();
	}

	glm::ivec2 World::ToTileIndices(int x, int y)
	{
		// convert from window coordinates into tiles indices
		glm::vec4 temp{ static_cast<float>(x), static_cast<float>(y), 0.f, 1.f };
		temp = (glm::inverse(m_view)) * temp;
		temp.x /= TILE_SIZE;
		temp.y /= TILE_SIZE;
		return { static_cast<int>(temp.x), static_cast<int>(temp.y) };
	}

	void World::PaintTileMouse(int mouseX, int mouseY, Tile type)
	{
		if (mouseX > m_winSize.first || mouseY > m_winSize.second)
			return;
		auto indices = ToTileIndices(mouseX, mouseY);
		PaintTileRaw(indices.x, indices.y, type);
	}

	void World::FillTileMouse(int mouseX, int mouseY, Tile type, int radius)
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

	void World::PaintTileRaw(int tileX, int tileY, Tile type)
	{
		if (tileX >= 0 && tileX < m_worldSize.first && tileY >= 0 && tileY < m_worldSize.second)
		{
			m_tileData[tileY * m_worldSize.first + tileX] = type;

			//geometry.reserve(m_worldSize.first * m_worldSize.second);

			int x = tileX;
			int y = tileY;
			Tile tileType = type;

			float size = 1.0f / (16);
			float uvx = (float)((unsigned char)tileType % 16) * size;
			float uvy = (float)((unsigned char)tileType / 16) * size;

			using cage::Vertex2UV;

			m_hackBuffer[0] = Vertex2UV(x * TILE_SIZE, y * TILE_SIZE, uvx, uvy);
			m_hackBuffer[1] = Vertex2UV((x + 1) * TILE_SIZE, y * TILE_SIZE, uvx + size, uvy);
			m_hackBuffer[2] = Vertex2UV(x * TILE_SIZE, (y + 1) * TILE_SIZE, uvx, uvy + size);

			m_hackBuffer[3] = Vertex2UV((x + 1) * TILE_SIZE, y * TILE_SIZE, uvx + size, uvy);
			m_hackBuffer[4] = Vertex2UV(x * TILE_SIZE, (y + 1) * TILE_SIZE, uvx, uvy + size);
			m_hackBuffer[5] = Vertex2UV((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE, uvx + size, uvy + size);

			m_worldMesh.GetBuffer().Bind();
			//for (int i = 0; i < 6; i++)
				//m_worldMesh.GetBuffer()[(m_worldSize.first * tileY + tileX) * 6 + i] = geometry[i];

			m_dirty = true;
			m_worldMesh.GetBuffer().UpdateRange((m_worldSize.first * tileY + tileX) * 6, m_hackBuffer);

		}
	}

	void World::GenWorld()
	{	
		std::vector<cage::Vertex2UV> geometry;
		geometry.reserve(m_worldSize.first * m_worldSize.second * 6);
		auto makeTile = [&geometry, this](int x, int y, unsigned char tileType) -> void
		{
			float size = 1.0f / (16);
			float uvx = (float)(tileType % 16) * size;
			float uvy = (float)(tileType / 16) * size;

			geometry.emplace_back(x * TILE_SIZE, y * TILE_SIZE, uvx, uvy);
			geometry.emplace_back((x + 1) * TILE_SIZE, y * TILE_SIZE, uvx + size, uvy);
			geometry.emplace_back(x * TILE_SIZE, (y + 1) * TILE_SIZE, uvx, uvy + size);

			geometry.emplace_back((x + 1) * TILE_SIZE, y * TILE_SIZE, uvx + size, uvy);
			geometry.emplace_back(x * TILE_SIZE, (y + 1) * TILE_SIZE, uvx, uvy + size);
			geometry.emplace_back((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE, uvx + size, uvy + size);
		};

		auto makeLine = [&geometry, this](int x1, int x2, int y1, int y2) -> void
		{
			geometry.emplace_back(static_cast<float>(TILE_SIZE) * glm::vec2{ x1, y1 }, glm::vec2{ 0.f, 0.f });
			geometry.emplace_back(static_cast<float>(TILE_SIZE) * glm::vec2{ x2, y2 }, glm::vec2{ 0.f, 0.f });
		};

		for (int y = 0; y < m_worldSize.second; y++)
		{
			for (int x = 0; x < m_worldSize.first; x++)
			{
				makeTile(x, y, (m_worldSize.first * y + x) % 0x30);
			}
		}

		m_worldMesh.GetBuffer().Fill(geometry);
		geometry.clear();
		for (int y = 0; y <= m_worldSize.second; y++)
		{
			makeLine(0, m_worldSize.first , y , y);
		}
		for (int x = 0; x <= m_worldSize.first; x++)
		{
			makeLine(x, x, 0, m_worldSize.second);
		}
		m_gridMesh.GetBuffer().Fill(geometry);

	}

	void World::SetWinSize(std::pair<int, int> winSize)
	{
		m_winSize = winSize;
		m_projection = glm::ortho(0.f, (float)winSize.first, (float)winSize.second, 0.f);
		m_view = calcViewMatrix();
	}

	void World::MoveEnt(unsigned int id, glm::vec2 position)
	{
		switch (id)
		{
		case 0:
			m_position = position;
			break;
		case 1:
			m_pos2 = position;
			break;
		}
	}

	void World::Update(float dt)
	{
		float speed = 256.f;
		m_velocity = { 0.0, 0.0 };

		auto keys = SDL_GetKeyboardState(NULL);
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
		m_cameraPos += (m_position + playerOffset - m_cameraPos) * moveTime * dt;

		m_position += m_velocity * dt;

		m_view = calcViewMatrix();

		m_scriptManager->Update(dt);

	}

	void World::Draw()
	{
		glDisable(GL_DEPTH_TEST);
		m_worldShader->Use();
		std::vector<cage::Vertex2UV> geometry;
		geometry.reserve(6);

		geometry.emplace_back(m_position.x + 0.f, m_position.y + 0.f, 0.f, 0.f);
		geometry.emplace_back(m_position.x + 28.f, m_position.y + 0.f, 0.f + 1, 0.f);
		geometry.emplace_back(m_position.x + 0.f, m_position.y + 48.f, 0.f, 1);

		geometry.emplace_back(m_position.x + 28.f, m_position.y + 0.f, 0.f + 1, 0.f);
		geometry.emplace_back(m_position.x + 0.f, m_position.y + 48.f, 0.f, 1);
		geometry.emplace_back(m_position.x + 28.f, m_position.y + 48.f, 1, 1);

		m_playerMesh.GetBuffer().Fill(geometry);

		geometry.clear();
		geometry.reserve(6);

		geometry.emplace_back(m_pos2.x + 0.f, m_pos2.y + 0.f, 0.f, 0.f);
		geometry.emplace_back(m_pos2.x + 28.f, m_pos2.y + 0.f, 0.f + 1, 0.f);
		geometry.emplace_back(m_pos2.x + 0.f, m_pos2.y + 48.f, 0.f, 1);

		geometry.emplace_back(m_pos2.x + 28.f, m_pos2.y + 0.f, 0.f + 1, 0.f);
		geometry.emplace_back(m_pos2.x + 0.f, m_pos2.y + 48.f, 0.f, 1);
		geometry.emplace_back(m_pos2.x + 28.f, m_pos2.y + 48.f, 1, 1);

		m_uberMesh.GetBuffer().Fill(geometry);


		m_worldShader->m_ViewProjection->value = m_projection * m_view;
		m_worldShader->m_ViewProjection->ForwardToShader();

		m_worldShader->m_Textured->value = true;
		m_worldShader->m_Textured->ForwardToShader();
		m_worldShader->m_Color->ForwardToShader();

		m_worldMesh.Draw();
		m_playerMesh.Draw();
		m_uberMesh.Draw();

		if (m_drawGrid)
		{
			m_worldShader->m_Textured->value = false;
			m_worldShader->m_Color->value = { 1.0, 1.0, 1.0, 0.3 };

			m_worldShader->m_Textured->ForwardToShader();
			m_worldShader->m_Color->ForwardToShader();

			m_gridMesh.Draw(GL_LINES);
		}

	}

	void World::HandleEvents(SDL_Event e)
	{
		switch (e.type)
		{
		case SDL_KEYDOWN:
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_RETURN:
				//GenWorld();
				break;
			}
			break;
		case SDL_MOUSEWHEEL:
			//if (e.wheel.y > 0)
			//	m_zoom *= 1.0 + 0.1;
			//else
			//	m_zoom *= 1.0 - 0.1;
			//m_zoom *= 1.0 - 0.1 * (((unsigned int)e.wheel.y & (1 << 31)) >> 31) + 0.1 * (1 - (((unsigned int)e.wheel.y & (1 << 31)) >> 31));
			Zoom(e.wheel.y);
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int w = e.window.data1;
				int h = e.window.data2;
				m_winSize = { w, h };
				m_projection = glm::ortho(0.f, (float)w, (float)h, 0.f);
			}
			break;
		}
	}
}