#pragma once
#include <memory>
#include <Core/BVH/BVHNode.h>
#include <Core/BVH/BVH.h>
#include <Core/Model.h>
namespace RT
{
	class Model;

	struct ReturnInfo
	{
		const RT::Triangle* triangle;
		float distance;
		std::shared_ptr<Model> model = nullptr;
	};
	
	class SceneBVH
	{
	public:
		void BuildBVH(std::vector<std::shared_ptr<Model>>& models);
		ReturnInfo Intersect(std::vector<std::shared_ptr<Model>>& models, const RT::Ray& ray, float& t) const;
		bool LightTraverse(const glm::vec3& lightpos, std::vector<std::shared_ptr<Model>>& models, const RT::Ray& ray) const;
	private:
		struct PartionInfo
		{
			eAxis SplitAxis;
			float SplittingCost;
			uint32_t Pivot;
			float SurfaceArea[2];
			float TotalSurfaceArea{ FP_INFINITE };
			std::array<glm::vec3, 2> LeftBounds;
			std::array<glm::vec3, 2> RightBounds;
		};

		struct SubDivideInfo
		{
			std::vector<std::shared_ptr<Model>>& Models;
			uint32_t Left;
			uint32_t& Poolptr;
			float CurrentNodeArea{ FP_INFINITE };
		};


		void Subdivide(RT::BVHNode& currentNode, SubDivideInfo info);
		PartionInfo Partion(std::array<glm::vec3, 2>& Bounds, std::vector<std::shared_ptr<Model>>& models, uint32_t first, uint32_t count, float currentCost);
		ReturnInfo TraverseBVH(std::vector<std::shared_ptr<Model>>& models, const BVHNode& currentNode, const RT::Ray& ray, float& boxDistance, float& objectDistance) const;
		BVHNode* m_nodes = nullptr;
		uint32_t m_numOfNodes{0};
		uint32_t m_splits = 4;
	};

}

