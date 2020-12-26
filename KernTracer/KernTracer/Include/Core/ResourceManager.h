#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <Core/ObjLoader.h>
#include <Core/Model.h>

namespace RT
{
	class ObjLoader;
	class BVH;
	class ResourceManager
	{
	public:
		ResourceManager();
		std::shared_ptr<RT::Model> LoadModel(const std::string& filePath);
	private:
		std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
		std::unordered_map<std::shared_ptr<Model>, std::shared_ptr<BVH>> m_BVH;
		std::unique_ptr<ObjLoader> m_objloader{};
	};
}


