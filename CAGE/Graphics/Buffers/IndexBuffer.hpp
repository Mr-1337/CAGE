#pragma once
#include <Glad/glad/glad.h>
#include <vector>

namespace cage
{


	class IndexBuffer
	{
	public:

		IndexBuffer(const std::vector<unsigned int>& indexData) : m_id(0), m_size(0)
		{
			glCreateBuffers(1, &m_id);
			Bind();
			m_size = indexData.size();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), &indexData[0], GL_STATIC_DRAW);
		}

		inline void Bind()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		}

		~IndexBuffer()
		{
			glDeleteBuffers(1, &m_id);
			UnbindAll();
		}

		inline size_t GetSize() const { return m_size; }

		inline unsigned int GetID() const { return m_id; }

		static void UnbindAll()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

	private:
		// OpenGL id or "name" for this ebo
		unsigned int m_id;

		size_t m_size;

	};
}