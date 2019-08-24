#pragma once
#include <GLAD/glad/glad.h>
#include <vector>

namespace cage
{

	template <class VertexType>
	class VertexBuffer
	{
	public:

		VertexBuffer(bool storeLocalCopy = false) : m_id(0), m_local(storeLocalCopy), m_size(0)
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
			m_size = data.size();
			if (m_local)
			{
				m_localData = data;
			}
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(VertexType), &data[0], GL_DYNAMIC_DRAW);
		}

		inline void Rebuffer()
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_localData.size() * sizeof(VertexType), &m_localData[0]);
		}

		~VertexBuffer()
		{
			glDeleteBuffers(1, &m_id);
		}

		VertexType& operator[](int i)
		{
			return m_localData[i];
		}

		inline unsigned int GetSize() const { return m_size; }

		inline unsigned int GetID() const { return m_id; }

	private:
		// OpenGL id or "name" for this vbo
		unsigned int m_id;

		// Local copy of the data if we want to hold it software side
		std::vector<VertexType> m_localData;

		// Should we store the vertex data software side?
		bool m_local;

		unsigned int m_size;

	};
}