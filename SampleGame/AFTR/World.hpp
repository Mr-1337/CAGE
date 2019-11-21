#include <array>

#include "../../CAGE/Graphics/Models/Model.hpp"

namespace aftr
{
	class World
	{
		std::array<cage::Model*, 7> m_models;
		//std::vector<int> m_
		const int SIZE;
	public:
		World(int size);


	};
}