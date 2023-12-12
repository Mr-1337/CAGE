#include "UIElement.hpp"
#include "../Vertex/Vertex.hpp"
#include "../VertexArrays/VertexArray.hpp"

#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage
{
	namespace ui
	{

		bool Vulkan = true;

		// shared geometry for all UIElement instances
		bool UIElement::init = false;
		std::shared_ptr<SpriteShader> UIElement::shader = nullptr;
		int UIElement::s_maskLayer = 0;
		std::vector<UIElement*> UIElement::s_DirtyTexture;

		Font* UIElement::s_DefaultFont = nullptr;

		VertexBuffer<Vertex2>* sharedVBO;
		VertexArray<Vertex2>* sharedVAO;

		unsigned int operator&(Axis lhs, Axis rhs)
		{
			return static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs);
		}

		unsigned int operator|(Axis lhs, Axis rhs)
		{
			return static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs);
		}

		bool IsHorizontal(Axis axes)
		{
			return (axes & Axis::HORIZONTAL) != 0;
		}

		bool IsVertical(Axis axes)
		{
			return (axes & Axis::VERTICAL) != 0;
		}


		void UIElement::initSharedData()
		{
			std::vector<cage::Vertex2> masterQuad;
			if (!Vulkan)
			{
				masterQuad.push_back(cage::Vertex2{ -0.5f, 0.5f });
				masterQuad.push_back(cage::Vertex2{ 0.5f, 0.5f });
				masterQuad.push_back(cage::Vertex2{ -0.5f, -0.5f });
				masterQuad.push_back(cage::Vertex2{ -0.5f, -0.5f });
				masterQuad.push_back(cage::Vertex2{ 0.5f, -0.5f });
				masterQuad.push_back(cage::Vertex2{ 0.5f, 0.5f });
				sharedVBO = new VertexBuffer<Vertex2>();

				sharedVBO->Fill(masterQuad);
				sharedVAO = new VertexArray<Vertex2>(*sharedVBO);
			}
			UIElement::init = true;
		}

		UIElement::UIElement() : UIElement(false)
		{

		}

		UIElement::UIElement(bool textured) : m_textured(textured), m_visible(true), m_masking(false), m_ignoreMasking(false)
		{
			if (!UIElement::init)
				UIElement::initSharedData();
			m_position = { 0.f, 0.f };
			m_relativePosition = { 0.f, 0.f };
			m_size = { 0.f, 0.f };
			m_relativeSize = { 0.f, 0.f };
			m_scale = { 1.f, 1.f };
			m_rotation = 0.f;
			m_totalTransform = glm::identity<glm::mat4>();
			m_mountOffset = { 0.f, 0.f };
			m_parentMountOffset = { 0.f, 0.f };
			m_color = { 0.f, 0.f, 0.f, 0.f };

			m_localMount = MountPoint::CENTER;
			m_parentMount = MountPoint::CENTER;
			m_relativeSizeAxes = Axis::NONE;
			m_relativePositionAxes = Axis::NONE;

			m_parent = nullptr;
		}

		void UIElement::LoadTexture(SDL_Surface* surface)
		{
			m_textured = true;
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
			if (!m_transformQueue.empty())
			{
				m_transformQueue.front()->UpdateLifetime(deltaTime);
				m_transformQueue.front()->Apply();
				if (m_transformQueue.front()->Complete())
				{
					m_transformQueue.pop();
					if (!m_transformQueue.empty())
						m_transformQueue.front()->Init();
				}
			}
			updateChildren(deltaTime);
		}

		void UIElement::Resize(glm::vec2 size)
		{
			bool horiz = IsHorizontal(m_relativeSizeAxes);
			bool vert = IsVertical(m_relativeSizeAxes);

			m_relativeSize.x = size.x * horiz;
			m_relativeSize.y = size.y * vert;

			if (!horiz)
				m_size.x = size.x;
			if (!vert)
				m_size.y = size.y;

			recalcTransform();
		}

		void UIElement::MoveTo(glm::vec2 newPosition)
		{
			bool horiz = IsHorizontal(m_relativePositionAxes);
			bool vert = IsVertical(m_relativePositionAxes);

			m_relativePosition.x = newPosition.x * horiz;
			m_relativePosition.y = newPosition.y * vert;

			if (!horiz)
				m_position.x = newPosition.x;
			if (!vert)
				m_position.y = newPosition.y;

			recalcTransform();
		}

		void UIElement::Scale(float scaleFactor)
		{
			m_scale *= scaleFactor;
			recalcTransform();
		}

		glm::vec2 UIElement::GetMountOffset(MountPoint mounting, glm::vec2 size, glm::vec2 scale)
		{
			switch (mounting)
			{
			case MountPoint::CENTER:		return { 0.f, 0.f };
			case MountPoint::CENTER_LEFT:	return { 0.5f * size.x * scale.x , 0.f };
			case MountPoint::CENTER_RIGHT:	return { -0.5f * size.x * scale.x , 0.f };
			case MountPoint::TOP:			return { 0.f , 0.5f * size.y * scale.y };
			case MountPoint::BOTTOM:		return { 0.f , -0.5f * size.y * scale.y };
			case MountPoint::TOP_LEFT:		return { 0.5f * size.x * scale.x,  0.5f * size.y * scale.y };
			case MountPoint::TOP_RIGHT:		return { -0.5f * size.x * scale.x,  0.5f * size.y * scale.y };
			case MountPoint::BOTTOM_LEFT:	return { 0.5f * size.x * scale.x, -0.5f * size.y * scale.y };
			case MountPoint::BOTTOM_RIGHT:	return { -0.5f * size.x * scale.x, -0.5f * size.y * scale.y };
			default:						return { 0.f, 0.f };
			}
		}

		void UIElement::onTransform()
		{

		}

		void UIElement::recalcTransform()
		{
			if (m_parent != nullptr)
			{
				if (IsHorizontal(m_relativeSizeAxes))
					m_size.x = m_parent->GetSize().x * m_relativeSize.x;
				if (IsVertical(m_relativeSizeAxes))
					m_size.y = m_parent->GetSize().y * m_relativeSize.y;
				if (IsHorizontal(m_relativePositionAxes))
					m_position.x = m_parent->GetSize().x * m_relativePosition.x;
				if (IsVertical(m_relativePositionAxes))
					m_position.y = m_parent->GetSize().y * m_relativePosition.y;
			}

			m_mountOffset = GetMountOffset(m_localMount, m_size, m_scale);
			if (m_parent != nullptr)
				m_parentMountOffset = -GetMountOffset(m_parentMount, m_parent->GetSize(), { 1.f, 1.f });

			auto local = glm::scale(glm::translate(glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(m_position + m_parentMountOffset, 0.f)), m_rotation, { 0.f, 0.f, 1.f }), { m_mountOffset, 0.f }), glm::vec3(m_scale, 1.0f));

			if (m_parent == nullptr)
			{
				m_totalTransform = local;
			}
			else
			{
				m_totalTransform = m_parent->GetTransform() * local;
			}
			for (auto c : m_children)
			{
				c->recalcTransform();
			}
			onTransform();
		}

		void UIElement::ScheduleTransform(std::unique_ptr<transforms::Transform> transform)
		{
			transform->SetElement(this);
			if (m_transformQueue.empty())
				transform->Init();
			m_transformQueue.push(std::move(transform));
		}

		bool UIElement::HaltCurrentTransform()
		{
			if (!m_transformQueue.empty())
				m_transformQueue.pop();
			return !m_transformQueue.empty();
		}

		void UIElement::HaltAllTransforms()
		{
			while (HaltCurrentTransform());
		}

		void UIElement::Add(Child child)
		{
			child->SetParent(this);
			child->SetDepth(GetDepth() + 1);
			child->recalcTransform();
			m_children.emplace_back(child);
		}

		void UIElement::Remove(Child child)
		{
			m_children.erase(std::remove(m_children.begin(), m_children.end(), child));
		}

		std::vector<UIElement::Child>& UIElement::GetChildren()
		{
			return m_children;
		}

		void UIElement::Draw()
		{
			if (m_masking)
			{
				glEnable(GL_STENCIL_TEST);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				s_maskLayer++;
				glStencilMask(0xFF);
			}

			if (m_parent != nullptr)
			{
				if (m_currentTexture)
					m_currentTexture->Bind();
				else
					Texture::s_MissingTexture->Bind();
				shader->Textured->value = m_textured;
				shader->Textured->ForwardToShader();
				shader->Color->value = m_color;
				shader->Color->ForwardToShader();
				sharedVAO->Bind();
				if (m_visible)
				{
					//if (m_ignoreMasking)
						//glDisable(GL_STENCIL_TEST);
					shader->SpriteSize->value = m_size;
					shader->SpriteSize->ForwardToShader();
					shader->Model->value = m_totalTransform;
					shader->Model->ForwardToShader();
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
			}
			if (m_masking)
			{
				glStencilFunc(GL_EQUAL, s_maskLayer, 0xFF);
				glStencilMask(0x00);
				drawChildren();
				glStencilMask(0xFF);
				glDisable(GL_STENCIL_TEST);
				s_maskLayer--;
			}
			else
			{
				if (m_visible)
					drawChildren();
			}
		}
	}

}