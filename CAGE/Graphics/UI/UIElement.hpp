#pragma once

#include <string>
#include <vector>
#include <memory>
#include <GLM/glm/glm.hpp>

#include "../../IO/Events/EventListener.hpp"
#include "../ShaderProgram/SpriteShader.hpp"
#include "../Textures/Texture.hpp"

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

		/*
		 * A CAGE UIElement is the base of all elements in the UI Scene Graph
		 * They can either be directly visible or simply represent aggregates of children UIElements
		 */

		class UIElement : public cage::EventListener
		{
		protected:

			using Child = std::shared_ptr<UIElement>;
		private:

			MountPoint m_localMount;
			MountPoint m_parentMount;

			UIElement* m_parent;
			std::vector<Child> m_children;

			glm::mat4 m_totalTransform;

			glm::vec2 m_position;
			// Size is the actual dimensions of the raw, untransformed UIElement which does not impact children, Scale is a scale transform that affects children 
			glm::vec2 m_scale, m_size, m_mountOffset, m_parentMountOffset;

			// The color to render this quad if it is a non textured element
			glm::vec4 m_color;

			float m_rotation;

		public:
			UIElement();
			UIElement(bool textured);
			UIElement(UIElement& other) = delete;
			UIElement(UIElement&& other) = delete;

			virtual void LoadTexture(SDL_Surface* surface);

			inline void SetActiveTexture(std::shared_ptr<Texture> texture)
			{
				if (texture)
				{
					m_textured = true;
					m_currentTexture = texture;
					Resize({ texture->GetSize().first, texture->GetSize().second });
				}
			}

			inline void SetColor(glm::vec4 color)
			{
				m_color = color;
			}

			inline virtual bool HandleEvent(Event& e)
			{
				return false;
			}

			virtual void Update(float deltaTime);
			virtual void Draw(); 


			// Sets what part of this element will be used as the local origin
			inline void SetLocalMounting(MountPoint mounting) { m_localMount = mounting; recalcTransform(); }

			// Sets what part of the parent element the origin should be placed at
			inline void SetParentMounting(MountPoint mounting) { m_parentMount = mounting; recalcTransform(); }

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
			inline void SetVisible(bool visible) { m_visible = visible; }
			inline void SetMasking(bool masking) { m_masking = masking; }
			
			inline glm::mat4 GetTransform() const { return m_totalTransform; }
			inline void SetTransform(const glm::mat4&& transform) { m_totalTransform = transform; }

			std::vector<Child>& GetChildren();

			// Adds a child to this UI element
			virtual void Add(Child child);

			inline void SetParent(UIElement* parent)
			{
				m_parent = parent;
			}

			static glm::vec2 GetMountOffset(MountPoint mounting, glm::vec2 size, glm::vec2 scale);

			virtual ~UIElement() = default;

			static std::shared_ptr<SpriteShader> shader;
			

		private:

			static void initSharedData();
			static bool init;

			bool m_textured;
			bool m_visible;
			bool m_masking;

			void recalcTransform();
			void drawChildren();
			void updateChildren(float deltaTime);

			static int s_maskLayer;

		protected:
			virtual void onTransform();



			std::shared_ptr<Texture> m_currentTexture;

		};
	}
}