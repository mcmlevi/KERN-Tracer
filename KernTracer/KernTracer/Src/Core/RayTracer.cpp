#include "pch.h"
#include <Core/RayTracer.h>
#include <Core/Triangle.h>
#include <Core/ObjLoader.h>
#include <Core/PointLight.h>
#include <glm/gtx/norm.hpp>
RT::ObjLoader loader;
std::unique_ptr<RT::Model> model = loader.LoadModel("cow.obj");
const RT::PointLight light{ {0,2,4},{1.f,1.f,1.f},50.f };

glm::vec3 RT::RayTracer::Trace(const Ray& ray, int currentDepth) const
{
	float distance{INFINITY};
	RT::Triangle* clostestTriangle{nullptr};
	bool hit{false};
	for (int i = 0; i < model->triangles.size(); ++i)
	{
		float currentDistance{};
		if (model->triangles[i].Intersect(ray, currentDistance))
		{
			if(currentDistance < distance)
			{
				distance = currentDistance;
				clostestTriangle = &model->triangles[i];
				hit = true;
			}
		}
	}
	if(hit)
	{
		return ApplyShading(distance * ray.direction + ray.origin, clostestTriangle, ray.origin);
	}
	else
	{
		return { 0.f,1.f,0.f };
	}

}

glm::vec3 RT::RayTracer::ApplyShading(const glm::vec3& hitPoint, const RT::Triangle* triangle, const glm::vec3& rayOrigin) const
{
	glm::vec3 out{};
	if (light.Intensity > 0)
	{
		out = { 1.f,0.f,0.f };
		glm::vec3 lightDirection = light.Position - hitPoint;
		float distance = glm::length2(lightDirection);
		lightDirection = glm::normalize(lightDirection);
		float NdotL = glm::dot(triangle->GetNormal(), lightDirection);
		float intensity = glm::clamp(NdotL, 0.f, 1.f);
		out *= intensity * light.Color * light.Intensity / distance;
		glm::vec3 half = glm::normalize(lightDirection + glm::normalize(rayOrigin - hitPoint));
		float NdotH = glm::dot(triangle->GetNormal(), half);
		intensity = glm::pow(glm::clamp(NdotH, 0.f, 1.f), 4);
		return out += (intensity * light.Color * light.Intensity / distance);
	}
	else
		return out;
}
