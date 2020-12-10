#include "pch.h"
#include "Core/Triangle.h"

#include <glm/ext/scalar_constants.hpp>

RT::Triangle::Triangle(const std::array<glm::vec3, 3>& verts):m_vertex{verts}
{
	
}

bool RT::Triangle::intersect(const Ray& ray, float& t)
{
	float u{}, v{};
	
	glm::vec3 v0v1 = m_vertex[1] - m_vertex[0];
	glm::vec3 v0v2 = m_vertex[2] - m_vertex[0];
	glm::vec3 pvec = glm::cross(ray.direction, v0v2);
	float det = glm::dot(v0v1, pvec);

	//if (det < epsilon) return false;
	if (fabs(det) < glm::epsilon<float>()) 
		return false;

	float invDet = 1.f / det;

	glm::vec3 tvec = ray.origin - m_vertex[0];
	u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	glm::vec3 qvec = glm::cross(tvec, v0v1);
	v = glm::dot(ray.direction, qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	t = glm::dot(v0v2, qvec) * invDet;
	if (t < 0)
		return  false;

	return true;
}
