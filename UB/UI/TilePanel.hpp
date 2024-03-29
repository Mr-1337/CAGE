#pragma once

#include "../../CAGE/Graphics/UI/Button.hpp"
#include "EditorPanel.hpp"
#include "../World/World.hpp"

namespace ub
{
	class TilePanel : public EditorPanel
	{
	public:
		TilePanel(World* world);
		Tilemap::Tile GetSelection();

		bool HandleEvent(cage::Event& e) override;

		inline std::string GetName() override
		{
			return "Tile Panel";
		}

	private:

		class TileButton : public cage::ui::Button
		{
		public:
			TileButton(Tilemap::Tile tile, std::optional<std::shared_ptr<cage::Texture>> idle) : cage::ui::Button(idle, std::nullopt, std::nullopt)
			{
			}
		private:

		};

		SDL_Surface* m_tileAtlas;
		Tilemap::Tile m_selection;

		void makeTileButton(Tilemap::Tile type);

	};
}