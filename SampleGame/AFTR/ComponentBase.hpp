#pragma once

#include <vector>

namespace aftr
{

	enum class Directions : char
	{
		NORTH = 1,
		EAST = 1 << 1,
		SOUTH = 1 << 2,
		WEST = 1 << 3,
		UP = 1 << 4,
		DOWN = 1 << 5
	};

	class ComponentBase
	{
		bool m_visited = false;
		bool m_state = false;

	protected:
		char m_connections = 0;
	public:
		virtual void Update() = 0;
		inline bool Visited() { return m_visited; }
		inline void Visit(bool visit) { m_visited = visit; }
		inline virtual bool GetState() { return m_state; }
		inline virtual void SetState(bool state) { m_state = state; }
		inline virtual Directions GetOrientation() = 0;
		inline bool HasConnection(Directions dir)
		{
			return m_connections & (char)dir;
		}
	public:
		ComponentBase* North = nullptr, * East = nullptr, * South = nullptr, * West = nullptr, * Up = nullptr, * Down = nullptr;
		
		// 0 0 1 1 1 1 1 1
		// 0 0 N E S W U D
	};
}
