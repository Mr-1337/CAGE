#include "Track.hpp"

void SplineSegment::calcSpline()
{
	float t01 = glm::pow(glm::distance(a, b), alpha);
	float t12 = glm::pow(glm::distance(b, c), alpha);
	float t23 = glm::pow(glm::distance(c, d), alpha);

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

	return { a * ttt + b * tt + c * t + d };
}

Track::Track()
{

}

Track::~Track()
{

}