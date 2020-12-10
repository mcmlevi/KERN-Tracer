#pragma once
#include <glm/vec3.hpp>
namespace RT
{
	struct Ray
	{
		Ray(const glm::vec3& o, const glm::vec3& d) :origin{ o }, direction{ d }{}
		glm::vec3 origin;
		glm::vec3 direction;
	};
}
