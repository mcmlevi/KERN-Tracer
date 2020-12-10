#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Triangle.h"
namespace RT
{
	struct Model
	{
		std::vector<Triangle> triangles;
	};

	
	class ObjLoader
	{
	public:
		ObjLoader() = default;
		std::unique_ptr<Model> LoadModel(const std::string& filePath);
		
	};
}


