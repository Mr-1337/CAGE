#pragma once

#include "ComponentBase.hpp"

namespace aftr
{
	class Network
	{
	public:
		Network(int worldSize);
		void Update(ComponentBase* startNode);
		bool Add(ComponentBase* node, int x, int y, int z);
		void Remove(int x, int y, int z);
		ComponentBase* GetNode(int index);
	private:
		void updateNet(ComponentBase* node, bool state);

		const int SIZE;
		std::vector<ComponentBase*> m_world;
		ComponentBase* entryNode = nullptr;
	};
}