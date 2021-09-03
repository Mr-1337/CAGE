#include "Node.hpp"

namespace cage
{
	namespace scene
	{
		Node::Node(const std::string& name) : m_name(name), m_LocalTransform(glm::identity<glm::mat4>()), m_InvTransform(m_LocalTransform), m_parent(nullptr)
		{
		}

		Node::~Node()
		{
			for (auto n : m_children)
			{
				// delete n;
			}
		}

		void Node::Add(Node* c)
		{
			c->m_parent = this;
			m_children.push_back(c);
		}

		glm::mat4 Node::GetTotalTransform()
		{
			//return glm::identity <glm::mat4>();
			if (m_parent)
				return m_parent->GetTotalTransform() * m_LocalTransform;
			else
				return m_LocalTransform;
		}
	}
}