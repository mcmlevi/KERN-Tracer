#include "pch.h"
#include <Core/BVH/SceneBVH.h>

void RT::SceneBVH::BuildBVH(std::vector<std::shared_ptr<Model>> models)
{
	int nodes = (2 * models.size() + 1);
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
		bounds[0] = glm::min(bounds[0], element->modelData->Bounds[0]);
		bounds[1] = glm::max(bounds[1], element->modelData->Bounds[1]);
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
}

const RT::Triangle* RT::SceneBVH::Intersect(std::vector<std::shared_ptr<Model>>, const RT::Ray& ray, float& t) const
{
	
}

bool RT::SceneBVH::LightTraverse(const glm::vec3& lightpos, std::vector<std::shared_ptr<Model>>,
	const RT::Ray& ray) const
{
	
}

void RT::SceneBVH::Subdivide(RT::BVHNode& currentNode, SubDivideInfo info)
{
	
}

RT::SceneBVH::PartionInfo RT::SceneBVH::Partion(std::array<glm::vec3, 2>& Bounds, RT::ModelData& model, uint32_t first,
	uint32_t count, float currentCost)
{
	
}
