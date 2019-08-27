#include "UIElement.hpp"
#include "../Vertex/Vertex.hpp"
#include "../VertexArrays/VertexArray.hpp"
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage::ui
{

	// shared geometry for all UIElement instances

	bool UIElement::init = false;
	SpriteShader* UIElement::shader = nullptr;

	VertexBuffer<Vertex2>* sharedVBO;
	VertexArray<Vertex2>* sharedVAO;

	void UIElement::initSharedData()
	{
		sharedVBO = new VertexBuffer<Vertex2>();

		std::vector<cage::Vertex2> masterQuad;
		masterQuad.emplace_back(cage::Vertex2{ -0.5f, 0.5f });
		masterQuad.emplace_back(cage::Vertex2{ 0.5f, 0.5f });
		masterQuad.emplace_back(cage::Vertex2{ -0.5f, -0.5f });
		masterQuad.emplace_back(cage::Vertex2{ -0.5f, -0.5f });
		masterQuad.emplace_back(cage::Vertex2{ 0.5f, -0.5f });
		masterQuad.emplace_back(cage::Vertex2{ 0.5f, 0.5f });

		sharedVBO->Fill(masterQuad);
		sharedVAO = new VertexArray<Vertex2>(*sharedVBO);

		UIElement::init = true;
	}

	UIElement::UIElement()
	{
		if (!UIElement::init)
			UIElement::initSharedData();
		m_position = { 0.f };
		m_size = { 0.f };
		m_scale = { 1.f };
		m_rotation = 0.f;
		m_localTransform = glm::identity<glm::mat4>();
		m_totalTransform = glm::identity<glm::mat4>();

		m_parent = nullptr;
	}

	void UIElement::LoadTexture(SDL_Surface* surface)
	{
		m_texture = std::make_unique<Texture>(surface, true);
		auto size = m_texture->GetSize();
		m_size = glm::vec2((float)size.first, (float)size.second);
	}

	void UIElement::updateChildren(float deltaTime)
	{
		for (auto& c : m_children)
		{
			c->Update(deltaTime);
		}
	}

	void UIElement::drawChildren()
	{
		for (auto& c : m_children)
		{
			c->Draw();
		}
	}

	void UIElement::Update(float deltaTime)
	{
	}

	void UIElement::Resize(glm::vec2 size)
	{
		m_scale = size;
		recalcTransform();
	}

	void UIElement::MoveTo(glm::vec2 newPosition)
	{
		m_position = newPosition;
		recalcTransform();
	}

	void UIElement::Scale(float scaleFactor)
	{
		m_scale *= scaleFactor;
		recalcTransform();
	}

	void UIElement::recalcTransform()
	{
		m_localTransform = glm::scale(glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(m_position + 0.5f * m_size, 0.f)), m_rotation, { 0.f, 0.f, 1.f }), glm::vec3(m_scale, 1.0f));
		if (m_parent == nullptr)
		{
			m_totalTransform = m_localTransform;
		}
		else
		{
			m_totalTransform = m_parent->GetTransform() * m_localTransform;
		}
	}

	void UIElement::Add(Child& child)
	{
		child->SetParent(this);
		m_children.emplace_back(child);
	}

	void UIElement::Draw()
	{
		drawChildren();
		if (m_texture)
			m_texture->Bind();
		sharedVAO->Bind();
		shader->SpriteSize->value = m_size;
		shader->SpriteSize->ForwardToShader();
		shader->Model->value = m_totalTransform;
		shader->Model->ForwardToShader();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

}