#include "pch.h"
#include <algorithm>
#include "Core/BVH/BVH.h"
#include <Core/Triangle.h>
#include <Core/Model.h>
void RT::BVH::BuildBVH(RT::ModelData& model)
{
	m_Nodes.resize(2 * model.triangles.size() + 1);
	m_Nodes[0].Bounds = model.Bounds;
	m_Nodes[0].count = static_cast<uint32_t>(model.triangles.size());
	m_Nodes[0].left = 0;
	//This is one since we set the root
	uint32_t poolptr{ 1 };
	// Calculate Area
	glm::vec3 boxSize{ glm::distance(m_Nodes[0].Bounds[0].x, m_Nodes[0].Bounds[1].x),
		glm::distance(m_Nodes[0].Bounds[0].y, m_Nodes[0].Bounds[1].y),
		glm::distance(m_Nodes[0].Bounds[0].z, m_Nodes[0].Bounds[1].z) };
	float BoxSurface{ 2 * boxSize.z * boxSize.y + 2 * boxSize.z * boxSize.x + 2 * boxSize.x * boxSize.y };
	SubDivideInfo info{ model,0,poolptr,BoxSurface };
	Subdivide(m_Nodes[0], info);
}

const RT::Triangle* RT::BVH::Intersect(const RT::Model& model, const RT::Ray& ray, float& t) const
{
	float boxDistance{ INFINITY };
	RT::Ray transformedRay{ model.transform.GetTransform() * glm::vec4{ray.origin,1.f},model.transform.GetTransform() * glm::vec4{ray.direction,0.f} };
	const RT::Triangle* triangle = TraverseBVH(*model.modelData.get(),m_Nodes[0], transformedRay, boxDistance, t);
	return triangle;
}
const RT::Triangle* RT::BVH::TraverseBVH(const RT::ModelData& model, const BVHNode& currentNode, const RT::Ray& ray, float& boxDistance, float& objectDistance) const
{
	// Exit if we miss the node
	float temp{};
	if(!currentNode.Intersect(ray,temp))
	{
		return nullptr;
	}
	// Check if the current node is a leaf node
	if(currentNode.count != 0)
	{
		const RT::Triangle* closestShape{nullptr};
		// Find closest object
		for (int i = 0; i < currentNode.count; ++i)
		{
			int index = i + currentNode.left;
			float distanceToTriange{INFINITY};
			if(model.triangles[index].Intersect(ray,distanceToTriange))
			{
				if (distanceToTriange < objectDistance)
				{
					objectDistance = distanceToTriange;
					closestShape = &model.triangles[index];
				}
			}
		}
		return closestShape;
	}
	// If current node is a branch node
	else
	{
		float DistanceLeftBox{};
		float DistanceRightBox{};
		const RT::Triangle* leftShape{nullptr};
		const RT::Triangle* rightShape{ nullptr };
		bool hitLeft = m_Nodes[currentNode.left].Intersect(ray, DistanceLeftBox);
		bool hitRight = m_Nodes[currentNode.left + 1].Intersect(ray, DistanceRightBox);

		if(hitLeft && hitRight)
		{
			leftShape = TraverseBVH(model, m_Nodes[currentNode.left], ray, boxDistance, objectDistance);
			rightShape = TraverseBVH(model, m_Nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		}
		else if(hitLeft)
		{
			leftShape = TraverseBVH(model, m_Nodes[currentNode.left], ray, boxDistance, objectDistance);
		}
		else if(hitRight)
		{
			rightShape = TraverseBVH(model, m_Nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		}
		//else
		//{
		//	if(DistanceLeftBox < 0 && DistanceRightBox < 0)
		//	{
		//		DistanceLeftBox *= -1;
		//		DistanceRightBox *= -1;
		//	}
		//	if(DistanceLeftBox < DistanceRightBox)
		//	{
		//		leftShape = TraverseBVH(model, m_Nodes[currentNode.left], ray, boxDistance, objectDistance);
		//		if(leftShape == nullptr)
		//			rightShape = TraverseBVH(model, m_Nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		//	}
		//	else
		//	{
		//		rightShape = TraverseBVH(model, m_Nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		//		if (rightShape == nullptr)
		//			leftShape = TraverseBVH(model, m_Nodes[currentNode.left], ray, boxDistance, objectDistance);
		//	}
		//}
		if (leftShape == nullptr && rightShape != nullptr)
		{
			return rightShape;
		}
		if (leftShape && rightShape)
		{
			float ld{ INFINITY };
			float rd{ INFINITY };
			bool lh = leftShape->Intersect(ray, ld);
			bool rh = rightShape->Intersect(ray, rd);
			if (rd < ld)
				return rightShape;
			else
				return leftShape;
		}
		return leftShape;
		
	}
}

void RT::BVH::Subdivide(RT::BVHNode& currentNode, SubDivideInfo& info)
{
	

	PartionInfo partionResult{ Partion(currentNode.Bounds,info.Model,info.Left,currentNode.count,info.CurrentNodeArea * currentNode.count) };
	// escape function
	if (partionResult.SplittingCost >= info.CurrentNodeArea * currentNode.count)
	{
		currentNode.first = info.Left;
		return;
	}
	
	RT::BVHNode& leftNode = m_Nodes[info.Poolptr];
	leftNode.Bounds = partionResult.LeftBounds;
	leftNode.left = info.Left;
	leftNode.count = partionResult.Pivot ;
	SubDivideInfo leftInfo{ info.Model,info.Left,info.Poolptr,partionResult.SurfaceArea[0] };
	currentNode.left = info.Poolptr++;

	RT::BVHNode& rightNode = m_Nodes[info.Poolptr];
	rightNode.Bounds = partionResult.RightBounds;
	rightNode.left = info.Left + partionResult.Pivot;
	rightNode.count = currentNode.count - partionResult.Pivot;
	SubDivideInfo rightInfo{info.Model,rightNode.left,info.Poolptr,partionResult.SurfaceArea[1]};
	info.Poolptr++;
	
	currentNode.count = 0;
	
	Subdivide(leftNode, leftInfo);
	Subdivide(rightNode, rightInfo);

	
}

RT::BVH::PartionInfo RT::BVH::Partion(std::array<glm::vec3, 2>& Bounds, RT::ModelData& model, uint32_t first, uint32_t count,float currentCost)
{
	PartionInfo info{eAxis::INVALID,INFINITY};
	int SplitIndex{};
	float cost{ currentCost };
	for (uint32_t axis = 0; axis < 3; ++axis)
	{
		float distance{ glm::distance(Bounds[0][axis],Bounds[1][axis]) };
		float inc{ distance / NUM_OF_SPLITS };
		std::sort(model.triangles.begin() + first, model.triangles.begin() + first + count, [axis](const RT::Triangle& a, const RT::Triangle b)
			{
				return a.GetCenter()[axis] < b.GetCenter()[axis];
			});
		for (uint32_t i = 1; i < NUM_OF_SPLITS ; ++i)
		{
			// Sort the objects based on the provided split axis
			float currentSplitPoint{ Bounds[0][axis] + i * inc };


			// Find then pivot point 
			uint32_t pivot{ 0 };
			glm::vec3 boundsLeft[2]{ glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY} };
			glm::vec3 boundsRight[2]{ glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY} };
			for (int triangle = first; triangle < first + count; ++triangle)
			{
				if(model.triangles[triangle].GetCenter()[axis] <= currentSplitPoint)
				{
					pivot++;
					// Calculate Bounds for left node
					const auto& verticeArray{ model.triangles[triangle].GetVerts() };
					for (int vertice = 0; vertice < 3; ++vertice)
					{
						boundsLeft[0] = glm::min(verticeArray[vertice], boundsLeft[0]);
						boundsLeft[1] = glm::max(verticeArray[vertice], boundsLeft[1]);
					}
				}
				else
				{
					// Calculate Bounds for right node
					const auto& verticeArray{ model.triangles[triangle].GetVerts() };
					for (int vertice = 0; vertice < 3; ++vertice)
					{
						boundsRight[0] = glm::min(verticeArray[vertice], boundsRight[0]);
						boundsRight[1] = glm::max(verticeArray[vertice], boundsRight[1]);
					}
				}
			}
			// Calulate Surface Area of the resulting 2 boxes
			// S = 2LH + 2LW + 2WH
			// X = Width
			// Y = Height
			// Z = Lenght
			glm::vec3 BL{ glm::distance(boundsLeft[0].x, boundsLeft[1].x),glm::distance(boundsLeft[0].y, boundsLeft[1].y),glm::distance(boundsLeft[0].z, boundsLeft[1].z) };
			glm::vec3 BR{ glm::distance(boundsRight[0].x, boundsRight[1].x),glm::distance(boundsRight[0].y, boundsRight[1].y),glm::distance(boundsRight[0].z, boundsRight[1].z) };
			float surfaceBL{2*BL.z * BL.y + 2 * BL.z * BL.x + 2 * BL.x * BL.y};
			float surfaceBR{ 2 * BR.z * BR.y + 2 * BR.z * BR.x + 2 * BR.x * BR.y };
			float totalSurfaceArea{surfaceBL + surfaceBR};
			float splittingCost{ surfaceBL * pivot + surfaceBR * (count - pivot) };

			if(splittingCost < cost)
			{
				if (pivot == 0)
					__debugbreak();
				cost = splittingCost;
				info.SplittingCost = splittingCost;
				info.SplitAxis = static_cast<eAxis>(axis);
				info.Pivot = pivot;
				info.TotalSurfaceArea = totalSurfaceArea;
				info.SurfaceArea[0] = surfaceBL;
				info.SurfaceArea[1] = surfaceBR;
				info.LeftBounds = { boundsLeft[0],boundsLeft[1] };
				info.RightBounds = { boundsRight[0],boundsRight[1] };
				SplitIndex = i;
			}
		}
	}
	// If we didn't partiotion we don't got to resort
	if (info.SplitAxis == eAxis::INVALID)
		return info;
	
	// Final Sort based on the optimal position
	std::sort(model.triangles.begin() + first, model.triangles.begin() + first + count, [&info](const RT::Triangle& a, const RT::Triangle b)
		{
			return a.GetCenter()[static_cast<uint32_t>(info.SplitAxis)] < b.GetCenter()[static_cast<uint32_t>(info.SplitAxis)];
		});
	printf("Info: Axis: %d, SplitIndex: %d \n", (int)info.SplitAxis,SplitIndex);
	return info;
}


