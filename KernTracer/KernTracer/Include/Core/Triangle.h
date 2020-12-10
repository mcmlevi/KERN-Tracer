#pragma once
#include <glm/vec3.hpp>
#include <array>

#include <Core/Ray.h>

namespace RT
{
	class Triangle
	{
	public:
		Triangle(const std::array<glm::vec3, 3>& verts);
		bool intersect(const Ray& ray, float& t);
	private:
		std::array<glm::vec3, 3> m_vertex;
	};

}


