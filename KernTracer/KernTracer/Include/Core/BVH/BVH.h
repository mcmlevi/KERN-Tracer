#pragma once
#include <vector>
#include <array>
#include <Core/Triangle.h>
#include <Core/Model.h>
#include <Core/BVH/BVHNode.h>

#define NUM_OF_SPLITS 16

namespace RT
{


	class BVH
	{
	public:
		BVH() = default;
		void BuildBVH(RT::Model& model);
		const RT::Triangle* Intersect(const RT::Model& model, const RT::Ray& ray, float& t) const;

		private:
			enum eAxis
			{
				X = 0,
				Y = 1,
				Z = 2,
				INVALID = 3,
			};

			struct PartionInfo
			{
				eAxis SplitAxis;
				float SplittingCost;
				uint32_t Pivot;
				float SurfaceArea[2];
				float TotalSurfaceArea{FP_INFINITE};
				std::array<glm::vec3, 2> LeftBounds;
				std::array<glm::vec3, 2> RightBounds;
			};

			struct SubDivideInfo
			{
				RT::Model& Model;
				uint32_t Left;
				uint32_t& Poolptr;
				float CurrentNodeArea{ FP_INFINITE };
			};

		void Subdivide(RT::BVHNode& currentNode, SubDivideInfo& info);
		PartionInfo Partion(std::array<glm::vec3,2>& Bounds, RT::Model& model,uint32_t first, uint32_t count, float currentCost);
		const RT::Triangle* TraverseBVH(const RT::Model& model, const BVHNode& currentNode, const RT::Ray& ray, float& boxDistance, float& objectDistance) const;
		std::vector<RT::BVHNode> m_Nodes;
		
	};

}