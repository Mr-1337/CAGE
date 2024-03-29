#pragma once

#include <GLM/glm/glm.hpp>
#include <string>

namespace cage
{
	namespace ui
	{

		class LayoutGroup;

		/* 
		 * Represents a layout configuration for a LayoutGroup. It will automatically position elements in a LayoutGroup and it will resize the LayoutGroup to fit all child elements
		 */
		class Layout
		{
		public:

			virtual ~Layout() {};

			void SetContainer(LayoutGroup* container)
			{
				m_container = container;
			}
			virtual void Update() = 0;
			

			virtual std::string GetName() = 0;
		protected:
			LayoutGroup* m_container;
		private:
		};
	}
}