#pragma once
#include <vector>
#include <memory>
#include <Core/BVH/SceneBVH.h>
namespace RT
{
	struct Model;
	struct PointLight;
	class Camera;
	class EnvironmentMap;

	struct Scene
	{
		std::vector<std::shared_ptr<RT::Model>> models;
		std::vector<std::shared_ptr<PointLight>> pointLights;
		std::vector<std::shared_ptr<Camera>> cameras;
		std::shared_ptr<Camera> activeCam;
		std::unique_ptr<RT::SceneBVH> sceneBvh = std::make_unique<RT::SceneBVH>();
		std::unique_ptr<RT::EnvironmentMap> environmentMap;
	};

}

