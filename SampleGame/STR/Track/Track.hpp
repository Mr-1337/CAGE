#pragma once

#include <GLM/glm/glm.hpp>

#include "../../../CAGE/Graphics/Models/Model.hpp"

class SplineSegment
{
	const float alpha = 0.5f;
	const float tension = 0.f;

	// cubic coefficient constants
	glm::vec3 a, b, c, d;


public:
	glm::vec3 p0, p1, p2, p3;

	glm::vec3 Interpolate(float t);
	void CalcSpline();

};

class Track
{
public:
	Track();
	~Track();

	cage::Mesh<cage::Vertex3UVNormal>& GetRoad();

private:
	cage::Mesh<cage::Vertex3UVNormal> m_roadMesh;
	SplineSegment m_spline;
};