#pragma once
#include <Core/Triangle.h>
#include <array>
namespace RT
{
	struct Model
	{
		std::vector<Triangle> triangles;
		std::array<glm::vec3, 2> Bounds{glm::vec3{FP_INFINITE,FP_INFINITE,FP_INFINITE},glm::vec3{-FP_INFINITE,-FP_INFINITE,-FP_INFINITE}};
		const RT::Triangle& operator[](unsigned i) const { return triangles[i]; }
		RT::Triangle& operator[](unsigned i) { return triangles[i];	}
	};
}
