#include "TilePanel.hpp"
#include "../CAGE/Graphics/UI/GridLayout.hpp"

namespace ub
{
	TilePanel::TilePanel(World* world) : 
		EditorPanel({ 200.f, 400.f }, world, "Tiles"),
		m_tileAtlas(nullptr),
		m_selection(Tilemap::Tile::INVALID)
	{

		m_content = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::GridLayout({ 10.f, 10.f }, 4));

		m_hoverHighlight->Resize({ Tilemap::TILE_SIZE, Tilemap::TILE_SIZE });
		m_hoverHighlight->Scale({ 1.2f });
		m_hoverHighlight->SetColor({ 1.0f, 0.f, 0.f, 0.5f });
		m_hoverHighlight->SetVisible(false);

		m_selectionHighlight->Resize({ Tilemap::TILE_SIZE, Tilemap::TILE_SIZE });
		m_selectionHighlight->Scale({ 1.2f });
		m_selectionHighlight->SetColor({ 0.0f, 1.f, 0.f, 0.5f });

		m_tileAtlas = IMG_Load("Assets/Textures/UBatlas.png");
		for (std::uint8_t i = 0; i < static_cast<std::uint8_t>(48); i++)
		{
			makeTileButton(static_cast<Tilemap::Tile>(i % 0x2f));
		}
		m_content->AddAbsolute(m_hoverHighlight);
		m_content->AddAbsolute(m_selectionHighlight);
		m_selectionHighlight->MoveTo(GetLayoutChildren()[0]->GetPosition());
		m_selectionHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		Add(m_content);
		SDL_FreeSurface(m_tileAtlas);
	}

	bool TilePanel::HandleEvent(cage::Event& e)
	{
		if (EditorPanel::HandleEvent(e))
			return true;
		for (auto c : m_content->GetChildren())
		{
			if (c->HandleEvent(e))
				return true;
		}
		return false;
	}

	Tilemap::Tile TilePanel::GetSelection()
	{
		return m_selection;
	}

	void TilePanel::makeTileButton(Tilemap::Tile type)
	{
		std::uint8_t asInt = static_cast<int>(type);
		int x, y;
		SDL_Rect src, dst;
		SDL_Surface* surface;
		
		surface = SDL_CreateRGBSurface(0, Tilemap::TILE_SIZE, Tilemap::TILE_SIZE, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		x = Tilemap::TILE_SIZE * (asInt % 16);
		y = Tilemap::TILE_SIZE * (asInt / 16);

		src.x = x;
		src.y = y;
		src.w = Tilemap::TILE_SIZE;
		src.h = Tilemap::TILE_SIZE;

		SDL_BlitSurface(m_tileAtlas, &src, surface, nullptr);

		const glm::vec2 offset = { 30.f, 30.f };

		auto button = std::make_shared<TileButton>(type, std::make_optional(std::make_shared<cage::Texture>(surface)));
		button->OnClick = [type, this, button]() { m_selection = type; m_selectionHighlight->MoveTo(button->GetPosition()); };
		button->OnHover = [this, button]() { m_hoverHighlight->SetVisible(true); m_hoverHighlight->MoveTo(button->GetPosition()); };
		button->OnUnHover = [this, button]() { m_hoverHighlight->SetVisible(false); };
		m_content->Add(button);

		SDL_FreeSurface(surface);
	}
}