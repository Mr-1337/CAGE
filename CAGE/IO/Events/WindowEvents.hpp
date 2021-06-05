#pragma once

namespace cage
{
	class WindowEvent
	{
	public:

		enum class Type
		{
			RESIZE = 0
		};

		Type type;
		int data1, data2;

		inline WindowEvent(Type type, int data1, int data2) :
			type(type), data1(data1), data2(data2)
		{
			
		}

	};
}