#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <Core/ObjLoader.h>
#include <Core/Model.h>

namespace RT
{
	struct Material;
	class ObjLoader;
	class BVH;
	class Texture;
	class ResourceManager
	{
	public:
		ResourceManager();
		std::shared_ptr<RT::ModelData> LoadModel(const std::string& filePath);
		std::shared_ptr<RT::Material> GetMaterial(const std::string& materialName);
		std::shared_ptr<RT::Texture> GetTexture(const std::string& filePath);
	private:
		std::unordered_map<std::string, std::shared_ptr<ModelData>> m_models;
		std::unordered_map<std::shared_ptr<ModelData>, std::shared_ptr<BVH>> m_BVH;
		std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
		std::unique_ptr<ObjLoader> m_objloader{};
	};
}


