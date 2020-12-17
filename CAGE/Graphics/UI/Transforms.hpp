#pragma once

#include <GLM/glm/glm.hpp>

namespace cage
{
	namespace ui
	{
		class UIElement;
		namespace transforms
		{
			enum class Interpolation
			{
				LINEAR,
				CUBIC
			};

			class Transform
			{
			protected:
				float m_startTime, m_endTime, m_lifeTime;
				bool m_complete;
				Interpolation m_interpolation;
				UIElement* m_element;

			public:

				Transform(float startTime, float endTime, Interpolation interpolation);

				void UpdateLifetime(float dt);

				void SetElement(UIElement* element)
				{
					m_element = element;
				}

				virtual void Init() = 0;

				virtual void Apply() = 0;

				inline bool Complete()
				{
					return m_complete;
				}

				float Interpolate();


			};

			class MoveTo : public Transform
			{
				glm::vec2 m_start, m_dst;
			public:
				MoveTo(glm::vec2 dst, float startTime, float endTime, Interpolation interpolation);
				void Init() override;
				void Apply() override;
			};

			class Move : public Transform
			{
				glm::vec2 m_start, m_dst;
			public:
				Move(glm::vec2 displacement, float startTime, float endTime, Interpolation interpolation);
				void Init() override;
				void Apply() override;
			};


		}
	}
}