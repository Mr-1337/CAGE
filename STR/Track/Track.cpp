#include "Track.hpp"
#include <iostream>

void SplineSegment::CalcSpline()
{
	float t01 = glm::pow(glm::distance(p0, p1), alpha);
	float t12 = glm::pow(glm::distance(p1, p2), alpha);
	float t23 = glm::pow(glm::distance(p2, p3), alpha);

	glm::vec3 m1 = (1.f - tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)));
	glm::vec3 m2 = (1.f - tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)));

	a = 2.0f * (p1 - p2) + m1 + m2;
	b = -3.0f * (p1 - p2) - m1 - m1 - m2;
	c = m1;
	d = p1;
}

glm::vec3 SplineSegment::Interpolate(float t)
{
	float ttt = t * t * t;
	float tt = t * t;

	return a * ttt + b * tt + c * t + d;
}

glm::vec3 SplineSegment::GetTangent(float t)
{
	float tt = t * t;

	return 3.0f * a * tt + 2.0f * b * t + c;
}

glm::vec3 SplineSegment::GetNormal(float t)
{
	return 6.0f * a * t + 2.0f * b;
}

Track::Track() : m_roadMesh("Road Mesh"), m_tangentMesh("Tangent Mesh"), m_normMesh("Normal Mesh"), m_selected(0)
{
	m_spline.p0 = { 2.5f, 9.f, 5.f };
	m_spline.p1 = { 4.f, 4.0f, 5.f };
	m_spline.p2 = { 7.5f, 4.f, 5.f };
	m_spline.p3 = { 10.f, 9., 5.f };

	regenMesh();

}


void Track::MoveNode(glm::vec3 translation)
{
	switch (m_selected)
	{
	case 0:
		m_spline.p0 += translation;
		break;
	case 1:
		m_spline.p1 += translation;
		break;
	case 2:
		m_spline.p2 += translation;
		break;
	case 3:
		m_spline.p3 += translation;
		break;
	}

	regenMesh();

}

glm::vec3 Track::GetSelectionOffset()
{
	switch (m_selected)
	{
	case 0:		return m_spline.p0;
	case 1:		return m_spline.p1;
	case 2:		return m_spline.p2;
	case 3:		return m_spline.p3;
	default:	return glm::vec3{ 0, 0, 0 };
	}
}

void Track::regenMesh()
{
	m_spline.CalcSpline();

	std::vector<cage::Vertex3UVNormal> points;
	std::vector<cage::Vertex3UVNormal> tanPoints;
	std::vector<cage::Vertex3UVNormal> normPoints;

	for (float t = 0.f; t <= 1.0f; t += 0.05f)
	{
		glm::vec3 normal = glm::normalize(m_spline.GetNormal(t));
		glm::vec3 tan = glm::normalize(m_spline.GetTangent(t));

		points.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t) - thickness * normal, {0.f, 0.f}, normal });

		tanPoints.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t), {0.f, 0.f},  {0.0, 0.0, 0.0} });
		tanPoints.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t) + 0.05f * tan, {0.f, 0.f},  normal });

		normPoints.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t), {0.f, 0.f},  {0.0, 0.0, 0.0} });
		normPoints.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t) + 0.05f * normal, {0.f, 0.f},  normal });
	}

	m_roadMesh.SetGeometry(points);
	m_tangentMesh.BindVAO();
	m_tangentMesh.SetGeometry(tanPoints);
	m_normMesh.BindVAO();
	m_normMesh.SetGeometry(normPoints);
}

Track::~Track()
{

}

cage::Mesh<cage::Vertex3UVNormal>& Track::GetRoad()
{
	return m_roadMesh;
}

cage::Mesh<cage::Vertex3UVNormal>& Track::GetTangentMesh()
{
	return m_tangentMesh;
}

cage::Mesh<cage::Vertex3UVNormal>& Track::GetNormalMesh()
{
	return m_normMesh;
}