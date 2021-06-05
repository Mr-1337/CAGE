#pragma once

#include <chrono>
#include <Graphics/UI/LayoutGroup.hpp>
#include <Graphics/UI/FlowLayout.hpp>
#include <Graphics/UI/Text.hpp>
#include <Audio/PlaybackEngine.hpp>


namespace ub
{

	class DialogueBox : public cage::ui::LayoutGroup
	{
	public:
		DialogueBox();

		void Say(const std::string& message, std::shared_ptr<cage::Texture> expressionTexture, std::shared_ptr<cage::audio::Sound> voice, const std::string& name = "");

		void Update(float dt) override;

		bool Complete();

		static cage::audio::PlaybackEngine* s_AudioEngine;
		static cage::Font* s_Font;
	private:
		std::shared_ptr<cage::audio::Sound> m_speakSound;
		std::shared_ptr<cage::ui::Text> m_text;
		std::string m_finalText, m_workingText;
		std::chrono::milliseconds m_textTime;
		std::chrono::duration<float> m_waitTimer;
	};

}
