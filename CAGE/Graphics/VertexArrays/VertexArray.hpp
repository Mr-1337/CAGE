#pragma once
#include "../Buffers/VertexBuffer.hpp"

namespace cage
{

	template <class VertexType>
	class VertexArray
	{
	public:

		VertexArray() : m_id(0), VBO(nullptr)
		{
			glCreateVertexArrays(1, &m_id);

			Bind();

			std::vector<VertexAttribute>& layout = GetLayout<VertexType>();
			for (int i = 0; i < layout.size(); i++)
			{
				glVertexAttribPointer(i, layout[i].dimensions, (GLenum)layout[i].type, layout[i].normalized, sizeof(VertexType), (const void*)layout[i].offset);
				glEnableVertexAttribArray(i);
			}
		}

		inline void Bind()
		{
			glBindVertexArray(m_id);
		}

		~VertexArray()
		{
			glDeleteVertexArrays(1, &m_id);
		}

		inline unsigned int GetID() const { return m_id; }
		VertexBuffer<VertexType>* VBO;

	private:
		// OpenGL id or "name" for this vao
		unsigned int m_id;
	};
}