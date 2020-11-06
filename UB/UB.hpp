#pragma once

#include <memory>
#include "../CAGE/CAGE.hpp"
#include "../CAGE/Core/StateMachine.hpp"
#include "World.hpp"

namespace ub
{
	class UndefinedBehavior : public cage::Game
	{
	public:
		UndefinedBehavior(int argc, char** argv);
		~UndefinedBehavior();

		void Run() override;

	private:
		bool m_running;
		cage::StateMachine m_stateMachine;

		void handleCMDArgs(int argc, char** argv) override;

	};
}