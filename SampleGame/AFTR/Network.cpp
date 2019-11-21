#include "Network.hpp"
#include "Well.hpp"
#include <iostream>
#include <algorithm>

namespace aftr
{

	Network::Network(int worldSize) : SIZE(worldSize)
	{
		m_world.resize(SIZE * SIZE * SIZE, nullptr);
	}

	void Network::Update(ComponentBase* startNode)
	{
		if (!startNode)
		{
			std::cout << "NO START NODE\n";
			return;
		}
		updateNet(startNode, startNode->GetState());
		std::for_each(m_world.begin(), m_world.end(), [](auto e) { if (e) e->Visit(false); });
	}

	void Network::updateNet(ComponentBase* node, bool state)
	{
		// exit if this node ain't real
		if (node == nullptr)
			return;
		// exit if we've already been here
		if (node->Visited())
			return;
		

		std::cout << "WE GOIN\n";
		node->SetState(state);
		node->Visit(true);
		bool lastState = node->GetState();
		updateNet(node->North, lastState);
		updateNet(node->East, lastState);
		updateNet(node->South, lastState);
		updateNet(node->West, lastState);
		updateNet(node->Up, lastState);
		updateNet(node->Down, lastState);
	}

	bool Network::Add(ComponentBase* node, int x, int y, int z)
	{
		if (m_world[(long long)z * SIZE * SIZE + y * SIZE + x] != nullptr)
		{
			std::cout << "SOMETHING IS HERE DOG!" << std::endl;
			return false;
		}
		m_world[(long long)z * SIZE * SIZE + y * SIZE + x] = node;

		if (node->HasConnection(aftr::Directions::DOWN))
		{
			node->Down = m_world[(long long)z * SIZE * SIZE + (y - 1) * SIZE + x];
			if (node->Down && node->Down->HasConnection(aftr::Directions::UP))
				node->Down->Up = node;
			else
				node->Down = nullptr;
		}

		if (node->HasConnection(aftr::Directions::UP))
		{
			node->Up = m_world[(long long)z * SIZE * SIZE + (y + 1) * SIZE + x];
			if (node->Up && node->Up->HasConnection(aftr::Directions::DOWN))
				node->Up->Down = node;
			else
				node->Up = nullptr;
		}

		if (node->HasConnection(aftr::Directions::NORTH))
		{
			node->North = m_world[(long long)(z + 1) * SIZE * SIZE + y * SIZE + x];
			if (node->North && node->North->HasConnection(aftr::Directions::SOUTH))
				node->North->South = node;
			else
				node->North = nullptr;
		}

		if (node->HasConnection(aftr::Directions::SOUTH))
		{
			node->South = m_world[(long long)(z - 1l) * SIZE * SIZE + y * SIZE + x];
			if (node->South && node->South->HasConnection(aftr::Directions::NORTH))
				node->South->North = node;
			else
				node->South = nullptr;
		}

		if (node->HasConnection(aftr::Directions::EAST))
		{
			node->East = m_world[(long long)z * SIZE * SIZE + y * SIZE + x + 1];
			if (node->East && node->East->HasConnection(aftr::Directions::WEST))
				node->East->West = node;
			else
				node->East = nullptr;
		}

		if (node->HasConnection(aftr::Directions::WEST))
		{
			node->West = m_world[(long long)z * SIZE * SIZE + y * SIZE + x - 1];
			if (node->West && node->West->HasConnection(aftr::Directions::EAST))
				node->West->East = node;
			else
				node->West = nullptr;
		}

		if (dynamic_cast<Well*>(node))
			entryNode = node;
		if (entryNode)
			Update(entryNode);
		return true;
	}

	void Network::Remove(int x, int y, int z)
	{

		auto node = m_world[(long long)z * SIZE * SIZE + y * SIZE + x];
		if (node == entryNode)
			entryNode = nullptr;

		if (!node)
			return;

		if (node->Down)
			node->Down->Up = nullptr;

		if (node->Up)
			node->Up->Down = nullptr;

		if (node->North)
			node->North->South = nullptr;

		if (node->South)
			node->South->North = nullptr;

		if (node->East)
			node->East->West = nullptr;

		if (node->West)
			node->West->East = nullptr;

		delete node;
		m_world[z * SIZE * SIZE + y * SIZE + x] = nullptr;
		node = nullptr;
		std::for_each(m_world.begin(), m_world.end(), [](auto e) { if (e) e->SetState(false); });
		if (entryNode)
			Update(entryNode);
	}

	ComponentBase* Network::GetNode(int index)
	{
		return m_world[index];
	}
}