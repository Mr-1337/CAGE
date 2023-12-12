#pragma once

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <algorithm>
#include <GLM/glm/glm.hpp>

#include "../Textures/VkTexture.hpp"
#include "../../IO/Events/EventListener.hpp"
#include "../ShaderProgram/SpriteShader.hpp"
#include "../Textures/Texture.hpp"
#include "Transforms.hpp"
#include "../Font/Font.hpp"
#include "../Buffers/VkVertexBuffer.hpp"

namespace cage
{
	namespace ui
	{

		enum class MountPoint
		{
			CENTER,
			CENTER_LEFT,
			CENTER_RIGHT,
			TOP,
			BOTTOM,
			TOP_LEFT,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT
		};

		enum class Axis : unsigned int
		{
			NONE = 0,
			HORIZONTAL = 1,
			VERTICAL = 2,
			BOTH = HORIZONTAL | VERTICAL
		};

		/*
		 * A CAGE UIElement is the base of all elements in the UI Scene Graph
		 * They can either be directly visible or simply represent aggregates of children UIElements
		 */

		bool IsHorizontal(Axis axes);
		bool IsVertical(Axis axes);


		class UIElement : public cage::EventListener
		{
		protected:

			using Child = std::shared_ptr<UIElement>;
		private:

			MountPoint m_localMount;
			MountPoint m_parentMount;
			
			Axis m_relativeSizeAxes;
			Axis m_relativePositionAxes;

			UIElement* m_parent;
			std::vector<Child> m_children;

			glm::mat4 m_totalTransform;

			glm::vec2 m_position, m_relativePosition;
			// Size is the actual dimensions of the raw, untransformed UIElement which does not impact children, Scale is a scale transform that affects children 
			glm::vec2 m_scale, m_size, m_relativeSize, m_mountOffset, m_parentMountOffset;

			// The color to render this quad if it is a non textured element
			glm::vec4 m_color;

			float m_rotation;

			std::queue<std::unique_ptr<transforms::Transform>> m_transformQueue;

		public:
			UIElement();
			UIElement(bool textured);
			UIElement(UIElement& other) = delete;
			UIElement(UIElement&& other) = delete;
			
			virtual ~UIElement() = default;

			virtual void LoadTexture(SDL_Surface* surface);

			inline virtual std::string GetName()
			{
				if (m_parent == nullptr)
					return "Root";
				else
					return "Unspecified";
			}

			virtual inline void SetActiveTexture(std::shared_ptr<Texture> texture)
			{
				if (texture)
				{
					m_textured = true;
					m_currentTexture = texture;
					Resize({ texture->GetSize().first, texture->GetSize().second });
				}
			}

			virtual inline void SetActiveTexture(std::shared_ptr<graphics::VkTexture> texture)
			{
				if (texture == nullptr) return;
				if (m_VkTexture != nullptr && m_VkTexture != texture)
				{
					s_DirtyTexture.push_back(this);
				}
				m_VkTexture = texture;
				Resize({ texture->GetSize().first, texture->GetSize().second });
			}

			inline glm::vec4 GetColor() const { return m_color; }

			virtual void SetColor(glm::vec4 color)
			{
				m_color = color;
			}

			inline bool IsVisible()
			{
				return m_visible;
			}

			inline virtual bool HandleEvent(Event& e)
			{
				if (!m_visible)
					return false;
				bool handled = false;
				for (auto c : m_children)
					handled |= c->HandleEvent(e);
				return handled;
			}

			virtual void Update(float deltaTime);
			virtual void Draw(); 

			// Sets what part of this element will be used as the local origin
			inline void SetLocalMounting(MountPoint mounting) { m_localMount = mounting; recalcTransform(); }

			// Sets what part of the parent element the origin should be placed at
			inline void SetParentMounting(MountPoint mounting) { m_parentMount = mounting; recalcTransform(); }

			// Sets along which axes this element should be sized relative to its parent (for example with axes set to horizontal and the size set to 0.5, the element would be half the width of the parent)
			inline void SetRelativeSizeAxes(Axis axes) { m_relativeSizeAxes = axes; }
			inline Axis GetRelativeSizeAxes() { return m_relativeSizeAxes; }

			// Sets along which axes this element should be positioned relative to its parent (for example with axes set to horizontal and position set to 0.5, this element would be positioned in the center of the parent horizontally)
			inline void SetRelativePositionAxes(Axis axes) { m_relativePositionAxes = axes; }

			inline MountPoint GetLocalMounting() { return m_localMount; }
			inline MountPoint GetParentMounting() { return m_parentMount; }

			inline glm::vec2 GetParentMountOffset() { return m_parentMountOffset; }

			// Transform manipulators and accessors

			void Resize(glm::vec2 size);
			void MoveTo(glm::vec2 newPosition);
			void Scale(float scaleFactor);
			inline glm::vec2 GetSize() const { return m_size; }
			inline glm::vec2 GetScale() const { return m_scale; }
			// Helper to get scaled size, useful for auto positioning elements relative to each other
			inline glm::vec2 GetScaledSize() const { return m_size * m_scale; }
			inline glm::vec2 GetPosition() const { return m_position; }
			inline void Rotate(float angle) { m_rotation += angle; recalcTransform(); }
			inline void SetRotation(float angle) { m_rotation = angle; recalcTransform(); }
			inline void SetScale(float scaleFactor) { m_scale = { scaleFactor, scaleFactor }; recalcTransform(); }
			inline void SetScale(glm::vec2 scale) { m_scale = scale; recalcTransform(); }
			inline void SetVisible(bool visible) { m_visible = visible; }
			inline void SetMasking(bool masking) { m_masking = masking; }
			inline void IgnoreMasking(bool ignores) { m_ignoreMasking = ignores; }

			void ScheduleTransform(std::unique_ptr<transforms::Transform> transform);
			bool HaltCurrentTransform();
			void HaltAllTransforms();
			
			inline glm::mat4 GetTransform() const { return m_totalTransform; }
			inline void SetTransform(const glm::mat4&& transform) 
			{ 
				m_totalTransform = transform;
				for (auto c : m_children)
				{
					c->recalcTransform();
				}
			}

			std::vector<Child>& GetChildren();

			// Adds a child to this UI element
			virtual void Add(Child child);

			virtual void Remove(Child child);

			inline void SetParent(UIElement* parent)
			{
				m_parent = parent;
			}

			static glm::vec2 GetMountOffset(MountPoint mounting, glm::vec2 size, glm::vec2 scale);


			static std::shared_ptr<SpriteShader> shader;
			static Font* s_DefaultFont;
			static std::vector<UIElement*> s_DirtyTexture;

			std::shared_ptr<graphics::VkTexture> m_VkTexture;
		private:

			static void initSharedData();
			static bool init;

			bool m_textured;
			bool m_visible;
			bool m_masking;
			bool m_ignoreMasking;

			void recalcTransform();
			void drawChildren();
			void updateChildren(float deltaTime);

			static int s_maskLayer;

		protected:
			virtual void onTransform();

			inline void setTextured(bool textured)
			{
				m_textured = textured;
			}

			std::shared_ptr<Texture> m_currentTexture;

		};
	}
}