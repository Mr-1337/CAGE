#pragma once

#include <string>
#include <map>

#include "UI/DialogueBox.hpp"
#include "Entity.hpp"

namespace ub
{

	class Character: public Entity 
	{
	public:

		Character(World* world, std::string name);
		~Character();

		void Update(float dt) override;
		bool OnInteract() override;

		bool IsDead();

		void Say(const std::string& message, const std::string& expression);

		void SetMaxHP(int hp);
		void SetHP(int hp);

		int GetHP();
		int GetMaxHP();

		void Damage(int dmg);

	private:

		int m_hp, m_maxHP;
		bool m_dead;
		void loadExpression(std::string filename);
		std::map<std::string, std::shared_ptr<cage::Texture>> m_expressions;
		std::shared_ptr<cage::audio::Sound> m_voice;
		std::shared_ptr<DialogueBox> m_dialogueBox;

	};

}
