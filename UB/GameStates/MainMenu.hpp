#pragma once

#include <array>
#include <../CAGE/Core/GameState.hpp>

#include "../CAGE/Graphics/UI/Button.hpp"
#include "../CAGE/IO/Assets/TextureManager.hpp"
#include "../CAGE/IO/Assets/SoundManager.hpp"
#include "../CAGE/Audio/PlaybackEngine.hpp"
#include <Graphics/UI/GridLayout.hpp>
#include <Graphics/UI/LayoutGroup.hpp>
#include "../CAGE/Graphics/Font/Font.hpp"

namespace ub
{
	class MainMenu : public cage::GameState
	{
	public:

		MainMenu(cage::Game& game);
		~MainMenu();

		void ProcessEvents() override;
		void Update(float dt) override;
		void Draw() override;

		void OnRevealed() override;
		void OnHidden() override;

	private:
		cage::ui::UIElement m_root;
		std::shared_ptr<cage::SpriteShader> m_shader;

		std::array<std::shared_ptr<cage::ui::Button>, 4> m_buttons;
		std::shared_ptr<cage::ui::UIElement> m_selector, m_debug;
		cage::Font m_font;

		enum class AudioChannels
		{
			INTERFACE,
			ENEMIES_NPCS,
			SFX,
			DIALOGUE,
			MUSIC
		};

		void updateSelector(int selection);
		unsigned int m_selection;

		std::chrono::milliseconds m_blinkTimer;

	};
}