#pragma once

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