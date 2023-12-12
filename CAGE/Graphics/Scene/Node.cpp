#include "Node.hpp"

namespace cage
{
	namespace graphics
	{

		NodeRef Node::Make(const std::string& name)
		{
			return std::make_shared<Node>(name);
		}

		Node::Node(const std::string& name) : m_name(name), m_LocalTransform(glm::identity<glm::mat4>()), m_GlobalTransform(m_LocalTransform), m_InvTransform(m_LocalTransform), m_parent(nullptr)
		{
		}

		Node::~Node()
		{

		}

		void Node::Add(NodeRef c)
		{
			c->m_parent = this;
			m_children.push_back(std::move(c));
		}

	}
}