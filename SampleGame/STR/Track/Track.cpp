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

Track::Track() : m_roadMesh("Road Mesh", true)
{
	m_spline.p0 = { -5.f, 2.f, -2.f };
	m_spline.p1 = { 0.f, 4.f, 2.f };
	m_spline.p2 = { 5.f, 3.f, 5.f };
	m_spline.p3 = { 10.f, 2.f, -2.f };

	m_spline.CalcSpline();

	auto p = m_spline.Interpolate(1.0);

	std::cout << p.x << ", " << p.y << ", " << p.z << std::endl;

	std::vector<cage::Vertex3UVNormal> points;

	for (float t = 0.f; t <= 1.0f; t += 0.01f)
	{
		points.emplace_back(cage::Vertex3UVNormal{ m_spline.Interpolate(t), {0.f, 0.f}, {0.f, 0.f, 0.f}});
	}

	m_roadMesh.SetGeometry(points);
}

Track::~Track()
{

}

cage::Mesh<cage::Vertex3UVNormal>& Track::GetRoad()
{
	return m_roadMesh;
}