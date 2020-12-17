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
	glm::vec3 GetTangent(float t);
	glm::vec3 GetNormal(float t);
	void CalcSpline();

};

class Track
{
public:
	Track();
	~Track();

	inline void Select(int n) { m_selected = n; }


	void MoveNode(glm::vec3 translation);
	glm::vec3 GetSelectionOffset();

	cage::Mesh<cage::Vertex3UVNormal>& GetRoad();
	cage::Mesh<cage::Vertex3UVNormal>& GetTangentMesh();
	cage::Mesh<cage::Vertex3UVNormal>& GetNormalMesh();

private:

	void regenMesh();

	cage::Mesh<cage::Vertex3UVNormal> m_roadMesh, m_tangentMesh, m_normMesh;
	SplineSegment m_spline;

	const float thickness = 0.0f;
	int m_selected;
};