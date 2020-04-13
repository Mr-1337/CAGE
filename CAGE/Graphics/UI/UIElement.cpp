#include "UIElement.hpp"
#include "../Vertex/Vertex.hpp"
#include "../VertexArrays/VertexArray.hpp"
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage::ui
{

	// shared geometry for all UIElement instances

	bool UIElement::init = false;
	std::shared_ptr<SpriteShader> UIElement::shader = nullptr;

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

	UIElement::UIElement() : UIElement(true)
	{

	}

	UIElement::UIElement(bool textured) : m_textured(textured)
	{
		if (!UIElement::init)
			UIElement::initSharedData();
		m_position = { 0.f };
		m_size = { 0.f };
		m_scale = { 1.f };
		m_rotation = 0.f;
		m_localTransform = glm::identity<glm::mat4>();
		m_totalTransform = glm::identity<glm::mat4>();
		m_mountOffset = { 0.f };
		m_pivot = { 0.f };

		m_parent = nullptr;
	}

	void UIElement::LoadTexture(SDL_Surface* surface)
	{
		m_currentTexture = std::make_shared<Texture>(surface, true);
		auto size = m_currentTexture->GetSize();
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
		//m_scale = size;
		m_size = size;
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

	glm::vec2 UIElement::getMountOffset()
	{
		switch (m_mountPoint)
		{
		case MountPoint::CENTER:		return { 0.f, 0.f };
		case MountPoint::TOP_LEFT:		return {  0.5f * m_size.x * m_scale.x,  0.5f * m_size.y * m_scale.y };
		case MountPoint::TOP_RIGHT:		return { -0.5f * m_size.x * m_scale.x,  0.5f * m_size.y * m_scale.y };
		case MountPoint::BOTTOM_LEFT:	return {  0.5f * m_size.x * m_scale.x, -0.5f * m_size.y * m_scale.y };
		case MountPoint::BOTTOM_RIGHT:	return { -0.5f * m_size.x * m_scale.x, -0.5f * m_size.y * m_scale.y };
		default:						return { 0.f, 0.f };
		}
	}

	void UIElement::onTransform()
	{

	}

	void UIElement::recalcTransform()
	{
		onTransform();
		m_mountOffset = getMountOffset();
		m_localTransform = glm::scale(glm::translate(glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(m_position + m_mountOffset, 0.f)), m_rotation, { 0.f, 0.f, 1.f }), { -m_pivot, 0.f }), glm::vec3(m_scale, 1.0f));
		if (m_parent == nullptr)
		{
			m_totalTransform = m_localTransform;
		}
		else
		{
			m_totalTransform = m_parent->GetTransform() * m_localTransform;
		}
	}

	void UIElement::Add(Child child)
	{
		child->SetParent(this);
		child->SetDepth(GetDepth() + 1);
		m_children.emplace_back(child);
	}

	std::vector<UIElement::Child>& UIElement::GetChildren()
	{
		return m_children;
	}

	void UIElement::Draw()
	{
		if (m_parent != nullptr)
		{
			if (m_currentTexture)
				m_currentTexture->Bind();
			else
				Texture::MissingTexture->Bind();
			sharedVAO->Bind();
			recalcTransform();
			if (m_textured)
			{
				shader->SpriteSize->value = m_size;
				shader->SpriteSize->ForwardToShader();
				shader->Model->value = m_totalTransform;
				shader->Model->ForwardToShader();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
		drawChildren();
	}

}