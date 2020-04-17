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

			MountPoint m_localMount;
			MountPoint m_parentMount;

			using Child = std::shared_ptr<UIElement>;

			UIElement* m_parent;
			std::vector<Child> m_children;

			glm::mat4 m_totalTransform;

			glm::vec2 m_position;
			// Size is the actual dimensions of the raw, untransformed UIElement which does not impact children, Scale is a scale transform that affects children 
			glm::vec2 m_scale, m_size, m_mountOffset, m_parentMountOffset;

			float m_rotation;

		public:
			UIElement();
			UIElement(bool textured);
			UIElement(UIElement& other) = delete;
			UIElement(UIElement&& other) = delete;

			virtual void LoadTexture(SDL_Surface* surface);

			inline void SetActiveTexture(std::shared_ptr<Texture> texture)
			{
				m_currentTexture = texture;
				m_size.x = texture->GetSize().first;
				m_size.y = texture->GetSize().second;
			}

			inline virtual bool HandleEvent(Event& e)
			{
				return false;
			}

			virtual void Update(float deltaTime);
			virtual void Draw(); 

			inline void SetLocalMounting(MountPoint mounting) { m_localMount = mounting; }
			inline void SetParentMounting(MountPoint mounting) { m_parentMount = mounting; }

			void Resize(glm::vec2 size);
			void MoveTo(glm::vec2 newPosition);
			void Scale(float scaleFactor);
			inline glm::vec2 GetSize() const { return m_size; }
			inline glm::vec2 GetScale() const { return m_scale; }
			inline glm::vec2 GetPosition() const { return m_position; }
			inline void Rotate(float angle) { m_rotation += angle; recalcTransform(); }
			inline void SetRotation(float angle) { m_rotation = angle; recalcTransform(); }
			inline void SetScale(float scaleFactor) { m_scale = { scaleFactor, scaleFactor }; recalcTransform(); }
			
			inline glm::mat4 GetTransform() const { return m_totalTransform; }

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

			void recalcTransform();
			void drawChildren();
			void updateChildren(float deltaTime);

		protected:
			virtual void onTransform();



			std::shared_ptr<Texture> m_currentTexture;

		};
	}
}