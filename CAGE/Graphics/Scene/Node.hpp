#pragma once

#include <vector>
#include <memory>
#include <string>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage
{
	namespace graphics
	{
		/*
			Represents a node in the CAGE scene graph.
			Stores transform information, and in the future bounding information for culling purposes.
		*/
		class Node;
		using NodeRef = std::shared_ptr<Node>;

		class Node
		{
			const std::string m_name;

			std::vector<NodeRef> m_children;
			Node* m_parent;

		public:

			static NodeRef Make(const std::string& name);

			Node(const std::string& name);
			~Node();
			void Add(NodeRef c);

			inline std::string GetName() { return m_name; }
			inline std::vector<NodeRef>& GetChildren() { return m_children; }
			inline Node* GetParent() { return m_parent; }

			glm::mat4 m_LocalTransform, m_GlobalTransform, m_InvTransform;

		};

	}
}