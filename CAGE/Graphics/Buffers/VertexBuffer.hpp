#pragma once
#include <GLAD/glad/glad.h>
#include <vector>

namespace cage
{

	template <class VertexType>
	class VertexBuffer
	{
	public:

		VertexBuffer(bool storeLocalCopy = false) : m_id(0), m_local(storeLocalCopy)
		{
			glCreateBuffers(1, &m_id);
		}

		inline void Bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
		}

		inline void Fill(std::vector<VertexType>& data)
		{
			Bind();
			if (m_local)
			{
				m_localData = data;
			}
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(VertexType), &data[0], GL_DYNAMIC_DRAW);
		}

		~VertexBuffer()
		{
			glDeleteBuffers(1, &m_id);
		}

		inline unsigned int GetID() const { return m_id; }

	private:
		// OpenGL id or "name" for this vbo
		unsigned int m_id;

		// Local copy of the data if we want to hold it software side
		std::vector<VertexType> m_localData;

		// Should we store the vertex data software side?
		bool m_local;

	};
}