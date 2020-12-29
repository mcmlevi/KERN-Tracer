#include "pch.h"
#include "Core/ResourceManager.h"
#include <Core/Model.h>
#include "Core/ObjLoader.h"
#include <Core/BVH/BVH.h>
#include <Core/Material.h>
#include <Graphics/Texture.h>
RT::ResourceManager::ResourceManager()
{
	m_objloader = std::make_unique<RT::ObjLoader>();
	m_materials["Default"] = std::make_shared<RT::Material>();
}

std::shared_ptr<RT::ModelData> RT::ResourceManager::LoadModel(const std::string& filePath)
{
	if (m_models.find(filePath) == m_models.end())
	{
		m_models[filePath] = m_objloader->LoadModel(filePath,*this);
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

std::shared_ptr<RT::Material> RT::ResourceManager::GetMaterial(const std::string& materialName)
{
	if (m_materials.find(materialName) == m_materials.end())
	{
		return m_materials[materialName] = std::make_shared<Material>();
	}
	else
		return m_materials[materialName];
}

std::shared_ptr<RT::Texture> RT::ResourceManager::GetTexture(const std::string& filePath)
{
	if (m_textures.find(filePath) == m_textures.end())
	{
		m_textures[filePath] = std::make_shared<Texture>();
		m_textures[filePath]->LoadTexture(filePath);
		return m_textures[filePath];
	}
	else
		return m_textures[filePath];
}
