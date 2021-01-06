#pragma once
#include <vector>
#include <array>
#include <mutex>
#include <atomic>
#include <Core/Triangle.h>
#include <Core/Model.h>
#include <Core/BVH/BVHNode.h>

#define NUM_OF_SPLITS 16

namespace RT
{

	enum eAxis
	{
		X = 0,
		Y = 1,
		Z = 2,
		INVALID = 3,
	};



	class Camera;
	class BVH
	{
	public:
		BVH() = default;
		~BVH();
		void BuildBVH(RT::ModelData& model);
		const RT::Triangle* Intersect(const RT::Model& model, const RT::Ray& ray, float& t) const;
		bool LightTraverse(const glm::vec3& lightpos, const RT::Model& model, const RT::Ray& ray) const;
		void Serialize(RT::ModelData& model, const std::string& rootDir, const std::string& modelName);
		bool ReadBVHFile(RT::ModelData& model, const std::string& rootDir, const std::string& modelName);
		void DrawBVH(const std::shared_ptr<Camera>& cam, const std::shared_ptr<Model>& model);
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
				RT::ModelData& Model;
				uint32_t Left;
				uint32_t& Poolptr;
				float CurrentNodeArea{ FP_INFINITE };
			};

		void Subdivide(RT::BVHNode& currentNode, SubDivideInfo info);
		PartionInfo Partion(std::array<glm::vec3,2>& Bounds, RT::ModelData& model,uint32_t first, uint32_t count, float currentCost);
		const RT::Triangle* TraverseBVH(const RT::ModelData& model, const BVHNode& currentNode, const RT::Ray& ray, float& boxDistance, float& objectDistance) const;
		bool TraverseBVHEarlyOut(const RT::ModelData& model, const BVHNode& currentNode, const RT::Ray& ray, const float lightDistance) const;
		void DrawNode(const BVHNode& node, const std::shared_ptr<Camera>& cam, const std::shared_ptr<Model>& model);
		RT::BVHNode* m_Nodes;
		uint32_t m_numOfNodes;
		std::mutex m_lock;
		std::atomic<int> m_counter;
	
	};

}
