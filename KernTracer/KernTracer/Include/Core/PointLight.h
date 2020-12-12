#pragma once
#include <glm/vec3.hpp>

namespace RT
{
	struct PointLight
	{
		PointLight(const glm::vec3& pos, const glm::vec3& color = { 1.f,1.f,1.f }, const float intensity = 1.f) :Position{ pos }, Color{ color }, Intensity{ intensity } {}
		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity;
	};
}
