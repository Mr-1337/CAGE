#pragma once

#include <string>
#include <vector>

#include "../VertexArrays/VertexArray.hpp"

namespace cage
{

	// A Mesh in CAGE represents a single collection of vertices associated with a texture and / or material
	template <class VertexType>
	class Mesh final
	{
		std::string m_name;
		VertexBuffer<VertexType> m_vbo;
		VertexArray<VertexType> m_vao;
	public:

		Mesh(const std::string& name, bool keepLocal) : m_name(name), m_vbo(keepLocal), m_vao(m_vbo)
		{

		}

		Mesh(const std::string& name) : Mesh(name, false)
		{

		}

		void SetGeometry(std::vector<VertexType>& geometry)
		{
			m_vbo.Fill(geometry);
		}

		inline std::string GetName() const { return m_name; }

		inline VertexBuffer<VertexType>& GetBuffer() { return m_vbo; }

		void Draw()
		{
			m_vao.Bind();
			glDrawArrays(GL_TRIANGLES, 0, m_vbo.GetSize());
		}

		void DrawIndexed(int count)
		{
			m_vao.Bind();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		}
	};
}