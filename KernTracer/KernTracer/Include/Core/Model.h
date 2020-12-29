#pragma once
#include <array>
#include <memory>

#include <Core/Triangle.h>
#include <Core/Transform.h>
#include <Core/Material.h>

namespace RT
{
	class BVH;
	struct Material;
	struct ModelData
	{
		std::vector<Triangle> triangles;
		std::shared_ptr<BVH> bvh;
		std::array<glm::vec3, 2> Bounds{ glm::vec3{FP_INFINITE,FP_INFINITE,FP_INFINITE},glm::vec3{-FP_INFINITE,-FP_INFINITE,-FP_INFINITE} };
		std::shared_ptr<Material> loadedInMaterial;
		const RT::Triangle& operator[](unsigned i) const { return triangles[i]; }
		RT::Triangle& operator[](unsigned i) { return triangles[i]; }
	};
	
	struct Model
	{
		std::shared_ptr<ModelData> modelData;
		Material material;
		RT::Transform transform{};
		const RT::Triangle& operator[](unsigned i) const { return modelData->operator[](i); }
		RT::Triangle& operator[](unsigned i) { return modelData->operator[](i);	}
	};
}
