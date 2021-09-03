#include "UIElement.hpp"

namespace cage::ui::transforms
{
	Transform::Transform(float startTime, float endTime, Interpolation interpolation) : m_startTime(startTime), m_endTime(endTime), m_lifeTime(0), m_complete(false), m_interpolation(interpolation), m_element(nullptr)
	{

	}

	void Transform::UpdateLifetime(float dt)
	{
		m_lifeTime += dt;
		m_complete = m_lifeTime > m_endTime - m_startTime;
	}

	float Transform::Interpolate()
	{
		float t = m_lifeTime / (m_endTime - m_startTime);
		switch (m_interpolation)
		{
		case Interpolation::LINEAR:
			return t;
			break;
		case Interpolation::CUBIC:
			return -2.0 * t * t * t + 3.0 * t * t;
			break;
		case Interpolation::BOUNCE_BACK:
		{
			const float c1 = 1.70158;
			const float c2 = 1.525 * c1;
			return t < 0.5f
				? (std::pow(2.f * t, 2.f) * ((c2 + 1.f) * 2.f * t - c2)) / 2.f
				: (std::pow(2.f * t - 2.f, 2.f) * ((c2 + 1.f) * (t * 2.f - 2.f) + c2) + 2.f) / 2.f;
			break;
		}
		default:
			return t;
		}
	}

	MoveTo::MoveTo(glm::vec2 dst, float startTime, float endTime, Interpolation interpolation) : Transform(startTime, endTime, interpolation), m_dst(dst)
	{

	}

	void MoveTo::Init()
	{
		m_start = m_element->GetPosition();
	}

	void MoveTo::Apply()
	{
		if (!Complete())
		{
			float t = Interpolate();
			glm::vec2 pos = m_start * (1 - t) + m_dst * t;
			m_element->MoveTo(pos);
		}
	}

	Move::Move(glm::vec2 displacement, float startTime, float endTime, Interpolation interpolation) : Transform(startTime, endTime, interpolation), m_dst(displacement)
	{

	}

	void Move::Init()
	{
		m_start = m_element->GetPosition();
		m_dst += m_element->GetPosition();
	}

	void Move::Apply()
	{
		if (!Complete())
		{
			float t = Interpolate();
			glm::vec2 pos = m_start * (1 - t) + (m_dst) * t;
			m_element->MoveTo(pos);
		}
	}

	FadeTo::FadeTo(glm::vec4 endColor, float startTime, float endTime) : Transform(startTime, endTime, Interpolation::LINEAR), m_end(endColor)
	{

	}

	void FadeTo::Init()
	{
		m_start = m_element->GetColor();
	}

	void FadeTo::Apply()
	{
		if (!Complete())
		{
			float t = Interpolate();
			m_element->SetColor(glm::mix(m_start, m_end, t));
		}
	}
}