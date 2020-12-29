#pragma once
#include <glm/vec3.hpp>
#include <array>

#include <Core/Ray.h>
#include <glm/vec2.hpp>

namespace RT
{
	class Triangle
	{
	public:
		Triangle(const std::array<glm::vec3, 3>& verts, 
			const std::array<glm::vec2, 3>& texturecoords = std::array<glm::vec2,3>{glm::vec2{}, glm::vec2{},glm::vec2{}});
		bool Intersect(const Ray& ray, float& t) const;
		const glm::vec3& GetNormal() const;
		glm::vec3 GetCenter() const;
		const std::array<glm::vec3, 3>& GetVerts() const;
		const std::array<glm::vec2, 3>& GetTextureCoords() const;
	private:
		std::array<glm::vec3, 3> m_vertex;
		std::array<glm::vec2, 3> m_textureCoords;
		glm::vec3 m_normal;
	};

}


