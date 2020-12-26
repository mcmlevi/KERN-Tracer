#include "pch.h"
#include "Core/ResourceManager.h"
#include <Core/Model.h>
#include "Core/ObjLoader.h"
#include <Core/BVH/BVH.h>

RT::ResourceManager::ResourceManager()
{
	m_objloader = std::make_unique<RT::ObjLoader>();
}

std::shared_ptr<RT::Model> RT::ResourceManager::LoadModel(const std::string& filePath)
{
	if (m_models.find(filePath) == m_models.end())
	{
		m_models[filePath] = m_objloader->LoadModel(filePath);
		m_BVH[m_models[filePath]] = std::make_shared<RT::BVH>();
		m_BVH[m_models[filePath]]->BuildBVH((*m_models[filePath].get()));
		m_models[filePath]->bvh = m_BVH[m_models[filePath]];
		return m_models[filePath];
	}
	else
	{
		return m_models[filePath];
	}
}
