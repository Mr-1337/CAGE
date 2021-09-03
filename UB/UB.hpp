#pragma once

// Some
// body once told me the
// world needs some OOP
// Adding classes to C would be rad

// He was
// prematurely bald and left
// totally appalled that his
// abomination was no fad

// Well, the years start coming and they don't stop coming
// Standard after standard and the madness keeps running
// Didn't make sense not to add any<T>
// But networking can wait 'til 2023

// So much to do
// So much to see
// So what's wrong with passing &T
// You'll never throw stack unwound woe
// You'll never take safety it's too slow


#include <memory>
#include "../CAGE/CAGE.hpp"
#include "../CAGE/Core/StateMachine.hpp"

namespace ub
{
	class UndefinedBehavior : public cage::Game
	{
	public:
		UndefinedBehavior();
		~UndefinedBehavior();

		void Init() override;
		void Run() override;

	private:
		bool m_running;
	};
}