#pragma once
#include <core/Ray.h>
namespace RT
{
	class RayTracer
	{
	public:
		RayTracer() = default;
		glm::vec3 Trace(const Ray& ray, int currentDepth);
	private:
		int m_maxDepth{ 3 };
	};
}


