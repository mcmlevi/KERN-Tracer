#include "pch.h"

#include <algorithm>
#include <Core/BVH/SceneBVH.h>

#include "Core/Timer.h"
#include <Graphics/OpenGL/glad.h>

std::array<glm::vec3,2> TransformBounds(const std::array<glm::vec3, 2>& bounds, const glm::mat4& transform)
{
	glm::vec3 corners[8];
	corners[0] = { bounds[0].x,bounds[0].y,bounds[0].z };
	corners[1] = { bounds[0].x,bounds[0].y,bounds[1].z };
	
	corners[2] = { bounds[0].x,bounds[1].y,bounds[0].z };
	corners[3] = { bounds[0].x,bounds[1].y,bounds[1].z };

	corners[4] = { bounds[1].x,bounds[0].y,bounds[0].z };
	corners[5] = { bounds[1].x,bounds[0].y,bounds[1].z };

	corners[6] = { bounds[1].x,bounds[1].y,bounds[0].z };
	corners[7] = { bounds[1].x,bounds[1].y,bounds[1].z };
	std::array<glm::vec3, 2> out{ glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY} };

	for (int i = 0; i < 8; ++i)
	{
		out[0] = glm::min(out[0], glm::vec3{ transform * glm::vec4{ corners[i],1.f }});
		out[1] = glm::max(out[1], glm::vec3{ transform * glm::vec4{ corners[i],1.f } });
	}
	return out;
}


void RT::SceneBVH::BuildBVH(std::vector<std::shared_ptr<Model>>& models)
{
	RT::Timer timer{};
	uint32_t nodes = (2 * models.size() + 1);
	if(nodes != m_numOfNodes)
	{
		if (!m_nodes)
			_aligned_free(m_nodes);

		m_numOfNodes = nodes;
		m_nodes = (RT::BVHNode*)_aligned_malloc(m_numOfNodes * sizeof(BVHNode), 32);
	}
	std::array<glm::vec3,2> bounds{glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY}};
	for (auto& element : models)
	{

		std::array<glm::vec3, 2> tBounds = TransformBounds(element->modelData->Bounds, element->transform.GetTransform());
		bounds[0] = glm::min(bounds[0], tBounds[0]);
		bounds[1] = glm::max(bounds[1], tBounds[1]);
	}
	m_nodes[0].Bounds = bounds;
	m_nodes[0].count = static_cast<uint32_t>(models.size());
	m_nodes[0].left = 0;
	//This is one since we set the root
	uint32_t poolptr{ 1 };
	// Calculate Area
	glm::vec3 boxSize{ glm::distance(m_nodes[0].Bounds[0].x, m_nodes[0].Bounds[1].x),
		glm::distance(m_nodes[0].Bounds[0].y, m_nodes[0].Bounds[1].y),
		glm::distance(m_nodes[0].Bounds[0].z, m_nodes[0].Bounds[1].z) };
	float BoxSurface{ 2 * boxSize.z * boxSize.y + 2 * boxSize.z * boxSize.x + 2 * boxSize.x * boxSize.y };
	SubDivideInfo info{ models,0,poolptr,BoxSurface };
	Subdivide(m_nodes[0], info);
	//printf("%f \n", timer.GetElapsedTimeInMS());
}

RT::ReturnInfo RT::SceneBVH::Intersect(const std::vector<std::shared_ptr<Model>>& models, const RT::Ray& ray, float& t) const
{
	assert(m_numOfNodes != 0);
	float boxDistance{ INFINITY };

	RT::ReturnInfo triangle = TraverseBVH(models, m_nodes[0], ray, boxDistance, t);
	return triangle;
}

bool RT::SceneBVH::LightTraverse(const glm::vec3& lightpos, const std::vector<std::shared_ptr<Model>>& models,
	const RT::Ray& ray) const
{
	assert(m_numOfNodes != 0);
	return TraverseWithEarlyOut(models, m_nodes[0], ray, lightpos);
}

void RT::SceneBVH::DrawBVH(const glm::mat4& projection, const glm::mat4& view, int drawDepth) const
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(1.f);
	glBegin(GL_LINES);

	TraverseDraw(m_nodes[0],projection,view,0,drawDepth);
		
	
	glEnd();
	
}

void RT::SceneBVH::Subdivide(RT::BVHNode& currentNode, SubDivideInfo info)
{
	PartionInfo partionResult = Partion(currentNode.Bounds,info.Models,info.Left,currentNode.count,info.CurrentNodeArea * currentNode.count);
	// escape function
	if (partionResult.SplittingCost >= info.CurrentNodeArea * currentNode.count )
	{
		currentNode.first = info.Left;
		return;
	}
	RT::BVHNode& leftNode = m_nodes[info.Poolptr];
	leftNode.Bounds = partionResult.LeftBounds;
	leftNode.left = info.Left;
	leftNode.count = partionResult.Pivot;
	SubDivideInfo leftInfo{ info.Models,info.Left,info.Poolptr,partionResult.SurfaceArea[0] };
	currentNode.left = info.Poolptr++;

	RT::BVHNode& rightNode = m_nodes[info.Poolptr];
	rightNode.Bounds = partionResult.RightBounds;
	rightNode.left = info.Left + partionResult.Pivot;
	rightNode.count = currentNode.count - partionResult.Pivot;
	SubDivideInfo rightInfo{ info.Models,rightNode.left,info.Poolptr,partionResult.SurfaceArea[1] };
	info.Poolptr++;

	currentNode.count = 0;

	Subdivide(leftNode, leftInfo);
	Subdivide(rightNode, rightInfo);
}

RT::SceneBVH::PartionInfo RT::SceneBVH::Partion(std::array<glm::vec3, 2>& Bounds, std::vector<std::shared_ptr<Model>>& models, uint32_t first,
	uint32_t count, float currentCost)
{
	PartionInfo info{ eAxis::INVALID,INFINITY };
	int SplitIndex{};
	float cost{ currentCost };
	for (uint32_t axis = 0; axis < 3; ++axis)
	{
		float distance{ glm::distance(Bounds[0][axis],Bounds[1][axis]) };
		float inc{ distance / m_splits };
		std::sort(models.begin() + first, models.begin() + first + count, [axis](std::shared_ptr<Model>& a, const std::shared_ptr<Model>& b)
			{
				std::array<glm::vec3, 2> aBounds = TransformBounds(a->modelData->Bounds, a->transform.GetTransform());
				std::array<glm::vec3, 2> bBounds = TransformBounds(b->modelData->Bounds, b->transform.GetTransform());
				return ((aBounds[0] + aBounds[1]) * 0.5f)[axis] < ((bBounds[0] + bBounds[1]) *0.5f)[axis];
			});

		for(const std::shared_ptr<Model>& model : models)
		{
			if (model.get() == nullptr)
				__debugbreak();
		}

		for (uint32_t i = 1; i < m_splits; ++i)
		{
			// Sort the objects based on the provided split axis
			float currentSplitPoint{ Bounds[0][axis] + i * inc };


			// Find then pivot point 
			uint32_t pivot{ 0 };
			glm::vec3 boundsLeft[2]{ glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY} };
			glm::vec3 boundsRight[2]{ glm::vec3{INFINITY,INFINITY,INFINITY},glm::vec3{-INFINITY,-INFINITY,-INFINITY} };
			for (unsigned model = first; model < first + count; ++model)
			{
				std::array<glm::vec3, 2> aBounds = TransformBounds(models[model]->modelData->Bounds, models[model]->transform.GetTransform());
				glm::vec3 center = (aBounds[0] + aBounds[1]) * 0.5f;
				if (center[axis] <=  currentSplitPoint)
				{
					pivot++;

					boundsLeft[0] = glm::min(aBounds[0], boundsLeft[0]);
					boundsLeft[1] = glm::max(aBounds[1], boundsLeft[1]);
				}
				else
				{
					// Calculate Bounds for right node
					boundsRight[0] = glm::min(aBounds[0], boundsRight[0]);
					boundsRight[1] = glm::max(aBounds[1], boundsRight[1]);
				}
			}
			// Calulate Surface Area of the resulting 2 boxes
			// S = 2LH + 2LW + 2WH
			// X = Width
			// Y = Height
			// Z = Lenght
			glm::vec3 BL{ glm::distance(boundsLeft[0].x, boundsLeft[1].x),glm::distance(boundsLeft[0].y, boundsLeft[1].y),glm::distance(boundsLeft[0].z, boundsLeft[1].z) };
			glm::vec3 BR{ glm::distance(boundsRight[0].x, boundsRight[1].x),glm::distance(boundsRight[0].y, boundsRight[1].y),glm::distance(boundsRight[0].z, boundsRight[1].z) };
			float surfaceBL{ 2 * BL.z * BL.y + 2 * BL.z * BL.x + 2 * BL.x * BL.y };
			float surfaceBR{ 2 * BR.z * BR.y + 2 * BR.z * BR.x + 2 * BR.x * BR.y };
			float totalSurfaceArea{ surfaceBL + surfaceBR };
			float splittingCost{ surfaceBL * pivot + surfaceBR * (count - pivot) };

			if (splittingCost < cost)
			{
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
	std::sort(models.begin() + first, models.begin() + first + count, [&info](std::shared_ptr<Model>& a, const std::shared_ptr<Model>& b)
		{
			std::array<glm::vec3, 2> aBounds = TransformBounds(a->modelData->Bounds, a->transform.GetTransform());
			std::array<glm::vec3, 2> bBounds = TransformBounds(b->modelData->Bounds, b->transform.GetTransform());
			return ((aBounds[0] + aBounds[1]) * 0.5f)[info.SplitAxis] < ((bBounds[0] + bBounds[1]) * 0.5f)[info.SplitAxis];
		});
	//printf("Info: Axis: %d, SplitIndex: %d \n", (int)info.SplitAxis, SplitIndex);
	return info;
}

RT::ReturnInfo RT::SceneBVH::TraverseBVH(const std::vector<std::shared_ptr<Model>>& models, const BVHNode& currentNode,
                                                   const RT::Ray& ray, float& boxDistance, float& objectDistance) const
{
	// Exit if we miss the node
	float temp{};
	if (!currentNode.Intersect(ray, temp))
	{
		return { nullptr,0};
	}
	// Check if the current node is a leaf node
	if (currentNode.count != 0)
	{
		const RT::Triangle* closestShape{ nullptr };
		std::shared_ptr<Model> model;
		// Find closest object
		for (uint32_t i = 0; i < currentNode.count; ++i)
		{
			int index = i + currentNode.left;
			float distanceToTriange{ INFINITY };
			glm::vec3 trd = glm::normalize(models[index]->transform.GetInverse() * glm::vec4{ ray.direction,0.f });
			glm::vec3 tro = models[index]->transform.GetInverse() * glm::vec4{ ray.origin,1.f };
			Ray transformedRay{ tro,trd };
			
			const RT::Triangle* hit = models[index]->modelData->bvh->Intersect(*models[index], transformedRay, distanceToTriange);
			if (hit)
			{
				glm::vec4 localHit = glm::vec4{ trd * distanceToTriange + tro ,1.f };				
				glm::vec3 worldIntersect = models[index]->transform.GetTransform() * localHit;
				distanceToTriange = glm::length(ray.origin - worldIntersect);
				if (distanceToTriange < objectDistance)
				{
					objectDistance = distanceToTriange;
					closestShape = hit;
					model = models[index];
				}
			}
		}
		return {closestShape,objectDistance,model};
	}
	// If current node is a branch node
	else
	{
		float DistanceLeftBox{};
		float DistanceRightBox{};
		RT::ReturnInfo leftShape{ nullptr};
		RT::ReturnInfo rightShape{ nullptr};
		bool hitLeft = m_nodes[currentNode.left].Intersect(ray, DistanceLeftBox);
		bool hitRight = m_nodes[currentNode.left + 1].Intersect(ray, DistanceRightBox);

		if (hitLeft || hitRight)
		{
			leftShape = TraverseBVH(models, m_nodes[currentNode.left], ray, boxDistance, objectDistance);
			rightShape = TraverseBVH(models, m_nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		}
		else if (hitLeft)
		{
			leftShape = TraverseBVH(models, m_nodes[currentNode.left], ray, boxDistance, objectDistance);
		}
		else if (hitRight)
		{
			rightShape = TraverseBVH(models, m_nodes[currentNode.left + 1], ray, boxDistance, objectDistance);
		}


		if (leftShape.triangle == nullptr && rightShape.triangle != nullptr)
		{
			return rightShape;
		}
		if (leftShape.triangle && rightShape.triangle)
		{

			if (rightShape.distance < leftShape.distance)
				return rightShape;
			else
				return leftShape;
		}
		return leftShape;

	}
}

bool RT::SceneBVH::TraverseWithEarlyOut(const std::vector<std::shared_ptr<Model>>& models, const BVHNode& currentNode,
	const RT::Ray& ray, const glm::vec3& lightPos) const
{
	// Exit if we miss the node
	float temp{};
	if (!currentNode.Intersect(ray, temp))
	{
		return false;
	}
	// Check if the current node is a leaf node
	if (currentNode.count != 0)
	{
		const RT::Triangle* closestShape{ nullptr };
		std::shared_ptr<Model> model;
		// Find closest object
		for (uint32_t i = 0; i < currentNode.count; ++i)
		{
			int index = i + currentNode.left;
			glm::vec3 trd = glm::normalize(models[index]->transform.GetInverse() * glm::vec4{ ray.direction,0.f });
			glm::vec3 tro = models[index]->transform.GetInverse() * glm::vec4{ ray.origin,1.f };
			Ray transformedRay{ tro,trd };

			bool hit = models[index]->modelData->bvh->LightTraverse(lightPos ,*models[index], transformedRay);
			if (hit)
			{
				return true;
			}
		}
		return false;
	}
	// If current node is a branch node
	else
	{
		float DistanceLeftBox{};
		float DistanceRightBox{};
		bool leftShape{ nullptr };
		bool rightShape{ nullptr };
		bool hitLeft = m_nodes[currentNode.left].Intersect(ray, DistanceLeftBox);
		bool hitRight = m_nodes[currentNode.left + 1].Intersect(ray, DistanceRightBox);

		if (hitLeft || hitRight)
		{
			leftShape = TraverseWithEarlyOut(models, m_nodes[currentNode.left], ray, lightPos);
			rightShape = TraverseWithEarlyOut(models, m_nodes[currentNode.left + 1], ray, lightPos);
		}
		else if (hitLeft)
		{
			leftShape = TraverseWithEarlyOut(models, m_nodes[currentNode.left], ray,lightPos);
		}
		else if (hitRight)
		{
			rightShape = TraverseWithEarlyOut(models, m_nodes[currentNode.left + 1], ray, lightPos);
		}
		if(leftShape || rightShape)
		{
			return true;
		}
		return false;
	}

	
}

void RT::SceneBVH::TraverseDraw(const BVHNode& currentNode, const glm::mat4& projection, const glm::mat4& view,int currentDepth, int drawDepth) const
{
	// skip all further nodes if they are never going to be drawn
	if(currentDepth > drawDepth)
		return;
	// Draw node
	if(currentDepth == drawDepth)
	{
		glm::vec3 corners[24];
		corners[0] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z }; // lower X back z
		corners[1] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z };

		corners[2] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z }; // lower X front z
		corners[3] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z };

		corners[4] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z }; // lower X back z
		corners[5] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z };

		corners[6] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z }; // lower X front z
		corners[7] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z };

		corners[8] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z }; // left y back z
		corners[9] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z };

		corners[10] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z }; // left y front z
		corners[11] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z };


		corners[12] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z }; // left y back z
		corners[13] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z };

		corners[14] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z }; // left y front z
		corners[15] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z };


		corners[16] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z }; // left z left x
		corners[17] = { currentNode.Bounds[0].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z };

		corners[18] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z }; // left z left x
		corners[19] = { currentNode.Bounds[0].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z };


		corners[20] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[0].z }; // right z right x
		corners[21] = { currentNode.Bounds[1].x,currentNode.Bounds[0].y,currentNode.Bounds[1].z };

		corners[22] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[0].z }; // right z right x
		corners[23] = { currentNode.Bounds[1].x,currentNode.Bounds[1].y,currentNode.Bounds[1].z };

		for (int j = 0; j < 24; ++j)
		{
			glm::vec4 p = projection * view * glm::vec4{ corners[j],1.f };
			p /= p.w;
			glVertex3f(p.x, p.y, p.z);
		}
	}
	// Termination if the node is a leaf
	if(currentNode.count != 0)
	{
		return;
	}

	TraverseDraw(m_nodes[currentNode.left], projection, view, currentDepth + 1, drawDepth);
	TraverseDraw(m_nodes[currentNode.left +1], projection, view, currentDepth + 1, drawDepth);
}
