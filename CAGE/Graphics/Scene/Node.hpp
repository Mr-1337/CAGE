#pragma once

#include <vector>
#include <memory>
#include <string>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage
{
	namespace scene
	{
		/*
			Represents a node in the CAGE scene graph
			Stores transform information
		*/
		class Node
		{
			const std::string m_name;

			std::vector<Node*> m_children;
			Node* m_parent;

		public:
			Node(const std::string& name);
			~Node();
			void Add(Node* c);

			inline std::string GetName() { return m_name; }
			glm::mat4 GetTotalTransform();

			glm::mat4 m_LocalTransform, m_InvTransform;
		};
	}
}