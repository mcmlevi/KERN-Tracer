#include "pch.h"
#include <Core/RayTracer.h>
#include <Core/Triangle.h>
#include <Core/ObjLoader.h>
#include <Core/PointLight.h>
#include <glm/gtx/norm.hpp>
#include <Core/BVH/BVH.h>
#include <Core/Scene.h>

RT::RayTracer::RayTracer(std::shared_ptr<Scene> sceneToRender)
{
	m_activeScene = sceneToRender;
}

void RT::RayTracer::ChangeSchene(std::shared_ptr<Scene> newSceneToRender)
{
	m_activeScene = newSceneToRender;
}

glm::vec3 RT::RayTracer::Trace(const Ray& ray, int currentDepth) const
{
	if (currentDepth >= m_maxDepth)
		return {};
	
	float distance{INFINITY};
	bool hit{ false };
	const RT::Triangle* clostestTriangle{nullptr};
	for (int i = 0; i < m_activeScene->models.size(); ++i)
	{
		float currentModelDistance{ INFINITY };
		const RT::Triangle* currentModelTriangle{ m_activeScene->models[i]->bvh->Intersect(*m_activeScene->models[i], ray,currentModelDistance) };
		if (currentModelTriangle && currentModelDistance < distance)
		{
			hit = true;
			distance = currentModelDistance;
			clostestTriangle = currentModelTriangle;
		}
	}
	
	if(hit)
	{
		//return { 1.f,0.f,0.f };
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
	for (int i = 0; i < m_activeScene->pointLights.size(); ++i)
	{
		glm::vec3 currentLightColor{};
		const RT::PointLight& light = *m_activeScene->pointLights[i];
		if (light.Intensity > 0)
		{
			currentLightColor = { 1.f,1.f,0.f };
			glm::vec3 lightDirection = light.Position - hitPoint;
			float distance = glm::length2(lightDirection);
			lightDirection = glm::normalize(lightDirection);
			glm::vec3 normal{ triangle->GetNormal() };
			float NdotL = glm::dot(normal, lightDirection);
			float intensity = glm::clamp(NdotL, 0.f, 1.f);
			currentLightColor *= intensity * light.Color * light.Intensity / distance;
			glm::vec3 half = glm::normalize(lightDirection + glm::normalize(rayOrigin - hitPoint));
			float NdotH = glm::dot(triangle->GetNormal(), half);
			intensity = glm::pow(glm::clamp(NdotH, 0.f, 1.f), 4);
			currentLightColor += (intensity * light.Color * light.Intensity / distance);
		}
		out += currentLightColor;
	}
	return out;
}
