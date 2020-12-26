#pragma once
#include <vector>
#include <string>
#include <memory>
#include <Core/Model.h>
namespace RT
{
	
	class ObjLoader
	{
	public:
		ObjLoader() = default;
		std::shared_ptr<Model> LoadModel(const std::string& filePath);
	};
}


