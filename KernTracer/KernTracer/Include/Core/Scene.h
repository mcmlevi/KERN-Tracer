#pragma once
#include <vector>
namespace RT
{
	struct Model;
	struct PointLight;
	class Camera;
	struct Scene
	{
		std::vector<std::shared_ptr<RT::Model>> models;
		std::vector<std::shared_ptr<PointLight>> pointLights;
		std::vector<std::shared_ptr<Camera>> cameras;
		std::shared_ptr<Camera> activeCam;
	};

}

