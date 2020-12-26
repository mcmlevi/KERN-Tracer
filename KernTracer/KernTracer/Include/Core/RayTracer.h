#pragma once
#include <core/Ray.h>
namespace RT
{
	class Triangle;

	class RayTracer
	{
	public:
		RayTracer();
		glm::vec3 Trace(const Ray& ray, int currentDepth) const;
	private:

		glm::vec3 ApplyShading(const glm::vec3& hitPoint, const RT::Triangle* triangle, const glm::vec3& rayOrigin) const;
		int m_maxDepth{ 3 };
	};
}


