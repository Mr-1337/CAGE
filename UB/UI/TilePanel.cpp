#include "TilePanel.hpp"
#include "../CAGE/Graphics/UI/GridLayout.hpp"

namespace ub
{
	TilePanel::TilePanel(World* world) : 
		EditorPanel(new cage::ui::GridLayout({ 15.f, 15.f }, 4), world, "Tiles"),
		m_tileAtlas(nullptr),
		m_selection(World::Tile::INVALID)
	{

		m_hoverHighlight->Resize({ World::TILE_SIZE, World::TILE_SIZE });
		m_hoverHighlight->Scale({ 1.2f });
		m_hoverHighlight->SetColor({ 1.0f, 0.f, 0.f, 0.5f });
		m_hoverHighlight->SetVisible(false);

		m_selectionHighlight->Resize({ World::TILE_SIZE, World::TILE_SIZE });
		m_selectionHighlight->Scale({ 1.2f });
		m_selectionHighlight->SetColor({ 0.0f, 1.f, 0.f, 0.5f });

		m_tileAtlas = IMG_Load("Assets/Textures/UBatlas.png");
		for (std::uint8_t i = 0; i < static_cast<std::uint8_t>(20); i++)
		{
			makeTileButton(static_cast<World::Tile>(i%11));
		}
		m_selectionHighlight->MoveTo(GetLayoutChildren()[0]->GetPosition());
		SDL_FreeSurface(m_tileAtlas);
	}

	bool TilePanel::HandleEvent(cage::Event& e)
	{
		if (auto se = std::get_if<cage::ScrollEvent>(&e))
		{
			auto position = GetPosition();
			//MoveTo(position + (32.f + 10.f) * glm::vec2{ 0.f, se->amount});
			return true;
		}
		return false;
	}

	World::Tile TilePanel::GetSelection()
	{
		return m_selection;
	}

	void TilePanel::makeTileButton(World::Tile type)
	{
		std::uint8_t asInt = static_cast<int>(type);
		int x, y;
		SDL_Rect src, dst;
		SDL_Surface* surface;
		
		surface = SDL_CreateRGBSurface(0, World::TILE_SIZE, World::TILE_SIZE, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		x = World::TILE_SIZE * (asInt % 16);
		y = World::TILE_SIZE * (asInt / 16);

		src.x = x;
		src.y = y;
		src.w = World::TILE_SIZE;
		src.h = World::TILE_SIZE;

		SDL_BlitSurface(m_tileAtlas, &src, surface, nullptr);

		const glm::vec2 offset = { 30.f, 30.f };

		auto button = std::make_shared<TileButton>(type, std::make_optional(std::make_shared<cage::Texture>(surface)));
		button->OnClick = [type, this, button]() { m_selection = type; m_selectionHighlight->MoveTo(button->GetPosition()); };
		button->OnHover = [this, button]() { m_hoverHighlight->SetVisible(true); m_hoverHighlight->MoveTo(button->GetPosition()); };
		button->OnUnHover = [this, button]() { m_hoverHighlight->SetVisible(false); };
		Add(button);

		SDL_FreeSurface(surface);
	}
}