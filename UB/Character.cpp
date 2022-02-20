#include "Character.hpp"
#include "World/World.hpp"
#include <filesystem>

namespace ub
{
	Character::Character(World* world, std::string name) :
		Entity(world, name),
		m_dialogueBox(world->GetDialogueBox()),
		m_hp(10),
		m_maxHP(10),
		m_dead(false)
	{
		loadExpression("annoyed");
		loadExpression("evil");
		loadExpression("stoic");
		loadExpression("happy");
		loadExpression("sus");
		loadExpression("icon");
		loadExpression("dguy");
		loadExpression("terry");
		loadExpression("stallman");
		loadExpression("gaben");
		loadExpression("shrek");
		loadExpression("dwight");

		std::filesystem::path file = std::filesystem::path("Characters") / GetName() / "speak.ogg";
		m_voice = s_Game->GetSoundManager().Get(file.string());
	}

	Character::~Character()
	{

	}

	void Character::Serialize(std::ostream& os)
	{

		const unsigned int size = 4 + GetName().length() + 1 + 4 + 4 + 1;
		unsigned int bufferPos = 0;
		char* buffer = new char[size];

		std::memcpy(buffer, &size, sizeof(unsigned int));
		bufferPos += sizeof(int);
		std::strcpy(&buffer[bufferPos], GetName().c_str());
		bufferPos += GetName().length() + 1;
		std::memcpy(&buffer[bufferPos], &m_hp, sizeof(int));
		bufferPos += sizeof(int);
		std::memcpy(&buffer[bufferPos], &m_maxHP, sizeof(int));
		bufferPos += sizeof(int);

		os.write(buffer, bufferPos);

		delete[] buffer;
	}

	void Character::SetHP(int hp)
	{
		m_hp = std::max(0, hp);
		if (m_hp == 0)
		{
			m_hp = 0;
			m_dead = true;
		}
	}

	void Character::SetMaxHP(int hp)
	{
		if (hp > 0)
			m_maxHP = hp;

		if (m_hp > m_maxHP)
			m_hp = m_maxHP;
	}

	int Character::GetHP()
	{
		return m_hp;
	}

	int Character::GetMaxHP()
	{
		return m_maxHP;
	}

	void Character::Damage(int dmg)
	{
		SetHP(GetHP() - dmg);
	}

	bool Character::IsDead()
	{
		return m_dead;
	}

	void Character::Update(float dt)
	{
		Entity::Update(dt);
	}

	bool Character::OnInteract()
	{
		if (!interacting())
		{
			startInteraction();
			Say("Hey, the other Dwight is pretty evil. You should kill him.", "dwight");
		}
		else if (m_dialogueBox->Complete())
		{
			m_dialogueBox->SetVisible(false);
			endInteraction();
		}
		return true;
	}

	void Character::Say(const std::string& message, const std::string& expression)
	{
		m_dialogueBox->SetVisible(true);
		m_dialogueBox->Say(message, m_expressions[expression], m_voice, GetName());
	}

	void Character::loadExpression(std::string filename)
	{
		std::filesystem::path file = std::filesystem::path("Characters") / GetName() / filename.append(".png");
		filename = filename.substr(0, filename.length() - 4);
		
		m_expressions[filename] = s_Game->GetTextureManager().Get(file.string());
	}
}