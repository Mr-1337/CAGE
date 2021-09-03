#pragma once
#include "../Buffers/VertexBuffer.hpp"
#include "../Buffers/IndexBuffer.hpp"
#include "../Vertex/Vertex.hpp"

namespace cage
{

	template <class VertexType>
	class VertexArray
	{
	public:

		VertexArray(VertexBuffer<VertexType>& vbo) : m_id(0), m_vbo(vbo)
		{
		
			vbo.Bind();

			glCreateVertexArrays(1, &m_id);

			Bind();

			const std::vector<VertexAttribute> layout = GetLayout<VertexType>();
			for (int i = 0; i < layout.size(); i++)
			{
				glVertexAttribPointer(i, layout[i].dimensions, (GLenum)layout[i].type, layout[i].normalized, sizeof(VertexType), (const void*)layout[i].offset);
				glEnableVertexAttribArray(i);
			}
		}

		VertexArray(const VertexArray<VertexType>& other) = delete;

		inline void Bind()
		{
			glBindVertexArray(m_id);
		}

		static void UnbindAll()
		{
			glBindVertexArray(0);
		}

		~VertexArray()
		{
			glDeleteVertexArrays(1, &m_id);
			UnbindAll();
		}

		inline unsigned int GetID() const { return m_id; }

	private:
		// OpenGL id or "name" for this vao
		unsigned int m_id;
		// The buffer linked with this VAO
		VertexBuffer<VertexType>& m_vbo;
	};
}