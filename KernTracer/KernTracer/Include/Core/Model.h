#pragma once
#include <Core/Triangle.h>
#include <array>
#include <memory>
namespace RT
{
	class BVH;
	struct Model
	{
		std::vector<Triangle> triangles;
		std::array<glm::vec3, 2> Bounds{glm::vec3{FP_INFINITE,FP_INFINITE,FP_INFINITE},glm::vec3{-FP_INFINITE,-FP_INFINITE,-FP_INFINITE}};
		std::shared_ptr<BVH> bvh;
		const RT::Triangle& operator[](unsigned i) const { return triangles[i]; }
		RT::Triangle& operator[](unsigned i) { return triangles[i];	}
	};
}
