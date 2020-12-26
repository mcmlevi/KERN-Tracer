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
		bool Intersect(const Ray& ray, float& t) const;
		const glm::vec3& GetNormal() const;
		glm::vec3 GetCenter() const;
		const std::array<glm::vec3, 3>& GetVerts() const;
	private:
		std::array<glm::vec3, 3> m_vertex;
		glm::vec3 m_normal;
	};

}


