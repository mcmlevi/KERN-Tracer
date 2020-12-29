#pragma once
#include <vector>
#include <string>
#include <memory>
#include <Core/Model.h>
namespace RT
{
	class ResourceManager;
	class ObjLoader
	{
	public:
		ObjLoader() = default;
		std::shared_ptr<ModelData> LoadModel(const std::string& filePath,ResourceManager& resourceManager);
	};
}


