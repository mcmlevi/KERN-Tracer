#pragma once
#include <string>
#include <Core/BVH/BVHNode.h>
#include <glm/vec2.hpp>

namespace RT
{
	struct Ray;
	class Texture;
	class EnvironmentMap
	{
	public:
		EnvironmentMap(const std::string& file);
		glm::vec3 Intersect(const RT::Ray& ray);
	private:

		//glm::vec2 calculateUV()
		BVHNode m_box;
		std::unique_ptr<Texture> m_Texture;
	};
}


