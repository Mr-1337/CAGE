#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL_image.h>
#include <memory>

#include "../VertexArrays/VertexArray.hpp"
#include "../Textures/Texture.hpp"

namespace cage
{

	// A Mesh in CAGE represents a single collection of vertices associated with a texture and / or material
	template <class VertexType>
	class Mesh final
	{
		std::string m_name;
		VertexBuffer<VertexType> m_vbo;
		VertexArray<VertexType> m_vao;
		std::shared_ptr<Texture> m_texture;
	public:

		Mesh(const std::string& name, bool keepLocal) : m_name(name), m_vbo(keepLocal), m_vao(m_vbo), m_texture(nullptr)
		{

		}

		Mesh(const std::string& name) : Mesh(name, false)
		{

		}

		~Mesh()
		{
		}

		void LoadTexture(SDL_Surface* surface)
		{
			m_texture = std::make_shared<Texture>(surface);
		}

		void LoadTexture(std::shared_ptr<Texture> texture)
		{
			m_texture = texture.get();
		}

		void SetGeometry(const std::vector<VertexType>& geometry)
		{
			m_vbo.Fill(geometry);
		}

		inline std::string GetName() const { return m_name; }

		inline VertexBuffer<VertexType>& GetBuffer() { return m_vbo; }

		inline void BindVAO() { m_vao.Bind(); }

		void Draw(GLenum primitive = GL_TRIANGLES)
		{
			if (m_texture)
				m_texture->Bind();
			else
				Texture::MissingTexture->Bind();
			m_vao.Bind();
			glDrawArrays(primitive, 0, m_vbo.GetSize());
		}

		void DrawIndexed(int count)
		{
			if (m_texture)
				m_texture->Bind();
			else
				Texture::MissingTexture->Bind();
			m_vao.Bind();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		}
	};
}