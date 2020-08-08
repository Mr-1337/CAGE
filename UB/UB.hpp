#include "../CAGE/CAGE.hpp"

namespace ub
{
	class UndefinedBehavior : public cage::Game
	{
	public:
		UndefinedBehavior(int argc, char** argv);
		~UndefinedBehavior();

		void Run() override;
	};
}