#include "DialogueBox.hpp"

using namespace cage;

namespace ub
{

	cage::audio::PlaybackEngine* DialogueBox::s_AudioEngine = nullptr;
	cage::Font* DialogueBox::s_Font = nullptr;

	DialogueBox::DialogueBox() : LayoutGroup(new ui::FlowLayout({ 20.0f, 20.0f })), m_waitTimer(0)
	{
		setTextured(false);
		SetColor({ 0.05f, 0.05f, 0.05f, 1.0f });
		SetLocalMounting(cage::ui::MountPoint::BOTTOM);
		SetParentMounting(cage::ui::MountPoint::BOTTOM);
		m_speakSound = nullptr;
		using namespace std::chrono_literals;
		m_textTime = 30ms;
	}

	void DialogueBox::Say(const std::string& message, std::shared_ptr<Texture> expressionTexture, std::shared_ptr<audio::Sound> voice, const std::string& name)
	{
		// Assign voice
		m_speakSound = voice;

		// Reset the state of the box
		for (auto c : GetLayoutChildren())
			Remove(c);

		// Add expression texture
		auto element = std::make_shared<ui::LayoutGroup>(new ui::FlowLayout(glm::vec2{ 0.0, 0.0 }, ui::FlowLayout::Orientation::VERTICAL, false));
		
		auto pic = std::make_shared<ui::UIElement>();
		pic->SetActiveTexture(expressionTexture);

		element->Add(pic);
		if (!name.empty())
		{
			auto nameSprite = std::make_shared<ui::Text>(*s_Font);
			nameSprite->SetText(name);
			element->Add(nameSprite);
		}

		Add(element);

		// Add text
		m_text = std::make_shared<cage::ui::Text>(*s_Font);
		Add(m_text);

		// Precalculate max dialogue box size
		m_text->SetText(message);
		LayoutGroup::Update();
		Compress();
		m_text->SetText("");

		// Prepare text algorithm
		m_finalText = message;
		m_workingText.clear();
		
	}

	void DialogueBox::Update(float dt)
	{
		m_waitTimer += std::chrono::duration<float>(dt);
		if (m_waitTimer >= m_textTime && !m_finalText.empty())
		{
			// Waited long enough to emit a new character

			m_workingText.append(m_finalText.substr(0, 1));
			m_finalText = m_finalText.substr(1);

			m_text->SetText(m_workingText);
			if (m_workingText.back() != ' ')
				s_AudioEngine->Play(*m_speakSound.get(), 1, 0);

			m_waitTimer = std::chrono::duration<float>(0);
		}
	}

	bool DialogueBox::Complete()
	{
		return m_finalText.empty();
	}
}