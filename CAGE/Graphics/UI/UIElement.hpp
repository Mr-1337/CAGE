#pragma once

#include <string>
#include <vector>
#include <memory>
#include <GLM/glm/glm.hpp>

#include "../ShaderProgram/SpriteShader.hpp"
#include "../Textures/Texture.hpp"

namespace cage
{
	namespace ui
	{
		class UIElement
		{
			using Child = std::shared_ptr<UIElement>;

			UIElement* m_parent;
			std::vector<Child> m_children;

			glm::mat4 m_localTransform, m_totalTransform;

			glm::vec2 m_position;
			// Size is the actual dimensions of the raw, untransformed UIElement which does not impact children, Scale is a scale transform that affects children 
			glm::vec2 m_scale, m_size;

			float m_rotation;

			std::unique_ptr<Texture> m_texture;

		public:
			UIElement();
			UIElement(UIElement& other) = delete;
			UIElement(UIElement&& other) = delete;

			void LoadTexture(SDL_Surface* surface);

			virtual void Update(float deltaTime);
			virtual void Draw(); 

			void Resize(glm::vec2 size);
			void MoveTo(glm::vec2 newPosition);
			void Scale(float scaleFactor);
			inline void Rotate(float angle) { m_rotation += angle; }
			
			inline glm::mat4 GetTransform() const { return m_totalTransform; }

			// Adds a child to this UI element
			void Add(Child& child);

			inline void SetParent(UIElement* parent)
			{
				m_parent = parent;
			}

			virtual ~UIElement() = default;

			static SpriteShader* shader;

		private:

			static void initSharedData();
			static bool init;

			void recalcTransform();

			void drawChildren();
			void updateChildren(float deltaTime);
		};
	}
}