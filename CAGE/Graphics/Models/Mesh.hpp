#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL_image.h>
#include <memory>

#include "../Scene/Node.hpp"
#include "../VertexArrays/VertexArray.hpp"
#include "../Textures/Texture.hpp"
#include "../ShaderProgram/Generic3DShader.hpp"

namespace cage
{

	// A Mesh in CAGE represents a single collection of vertices associated with a texture and / or material
	template <class VertexType>
	class Mesh final
	{
		std::string m_name;
		VertexBuffer<VertexType> m_vbo;
		IndexBuffer* m_ebo;
		VertexArray<VertexType> m_vao;
		std::shared_ptr<Texture> m_texture;


	public:

		Mesh(const std::string& name, bool keepLocal) : m_name(name), m_vbo(keepLocal), m_ebo(nullptr), m_vao(m_vbo), m_texture(nullptr), m_TransformNode(nullptr)
		{
		}

		Mesh(const std::string& name) : Mesh(name, false)
		{

		}

		scene::Node* m_TransformNode;

		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;

		~Mesh()
		{

		}

		void LoadTexture(SDL_Surface* surface)
		{
			m_texture = std::make_shared<Texture>(surface);
		}

		void LoadTexture(std::shared_ptr<Texture> texture)
		{
			m_texture = texture;
		}

		void SetGeometry(const std::vector<VertexType>& geometry)
		{
			m_vbo.Fill(geometry);
		}

		void SetGeometry(const std::vector<VertexType>& geometry, const std::vector<unsigned int>& indexData)
		{
			if (m_ebo)
				delete m_ebo;
			BindVAO();
			m_vbo.Fill(geometry);
			m_ebo = new IndexBuffer(indexData);
			m_ebo->Bind();
		}

		inline std::string GetName() const { return m_name; }

		inline VertexBuffer<VertexType>& GetBuffer() { return m_vbo; }

		inline void BindVAO() { m_vao.Bind(); }

		void Draw(GLenum primitive = GL_TRIANGLES)
		{
			if (m_texture)
				m_texture->Bind();
			else
				Texture::s_MissingTexture->Bind();
			m_vao.Bind();
			glDrawArrays(primitive, 0, m_vbo.GetSize());
		}

		void Draw(std::shared_ptr<Generic3DShader> shader)
		{
			if (m_texture)
				m_texture->Bind();
			else
				Texture::s_MissingTexture->Bind();

			shader->Model->value = m_TransformNode->GetTotalTransform();
			shader->Model->ForwardToShader();

			m_vao.Bind();
			glDrawArrays(GL_TRIANGLES, 0, m_vbo.GetSize());
		}

		void DrawIndexed(std::shared_ptr<Generic3DShader> shader)
		{
			if (m_texture)
				m_texture->Bind();
			else
				Texture::s_MissingTexture->Bind();

			shader->Model->value = m_TransformNode->GetTotalTransform();
			shader->Model->ForwardToShader();

			m_vao.Bind();
			if (m_ebo)
				glDrawElements(GL_TRIANGLES, m_ebo->GetSize(), GL_UNSIGNED_INT, nullptr);
			else
				glDrawArrays(GL_TRIANGLES, 0, m_vbo.GetSize());
		}
	};
}