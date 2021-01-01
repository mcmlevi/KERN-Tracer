#include "pch.h"
#include <Core/RayTracer.h>
#include <Core/Triangle.h>
#include <Core/ObjLoader.h>
#include <Core/PointLight.h>
#include <glm/gtx/norm.hpp>
#include <Core/BVH/BVH.h>
#include <Core/Scene.h>
#include <Core/Material.h>
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

	HitInfo hitInfo;
	hitInfo.Distance = INFINITY;
	hitInfo.Hit = false;
	for (size_t i = 0; i < m_activeScene->models.size(); ++i)
	{
		float currentModelDistance{ INFINITY };
		const auto& modelref = m_activeScene->models[i];
		glm::vec3 trd = glm::normalize(modelref->transform.GetInverse() * glm::vec4{ ray.direction,0.f });
		glm::vec3 tro =  modelref->transform.GetInverse() * glm::vec4{ ray.origin,1.f };
		Ray transformedRay{ tro,trd };
		const RT::Triangle* currentModelTriangle{ m_activeScene->models[i]->modelData->bvh->Intersect(*m_activeScene->models[i], transformedRay,currentModelDistance) };
		if (currentModelTriangle)
		{
			glm::vec4 localHit = glm::vec4{ trd * currentModelDistance + tro ,1.f };
			
			glm::vec3 worldIntersect = modelref->transform.GetTransform() * localHit;
			
			currentModelDistance = glm::length(ray.origin - worldIntersect);
			if(currentModelDistance < hitInfo.Distance)
			{
				hitInfo.Hit = true;
				hitInfo.LocalIntersect = localHit;
				hitInfo.WorldIntersect = worldIntersect;
				hitInfo.Distance = currentModelDistance;
				hitInfo.HitTriangle = currentModelTriangle;
				hitInfo.Material = &m_activeScene->models[i]->material;
				hitInfo.HitModel = m_activeScene->models[i];
			}
		}
	}
	
	if(hitInfo.Hit)
	{
		//return { 1.f,0.f,0.f };
		return ApplyShading(hitInfo, ray.origin);
	}
	else
	{
		return { 0.f,1.f,0.f };
	}

}

glm::vec3 RT::RayTracer::ApplyShading(const HitInfo& hitInfo, const glm::vec3& rayOrigin) const
{
	glm::vec3 out{};
	for (size_t i = 0; i < m_activeScene->pointLights.size(); ++i)
	{
		glm::vec3 currentLightColor{};
		const RT::PointLight& light = *m_activeScene->pointLights[i];
		if (light.Intensity > 0)
		{
			glm::vec3 texColor{ hitInfo.Material->GetTexture(hitInfo.LocalIntersect,*hitInfo.HitTriangle) };
			currentLightColor = hitInfo.Material->baseColor * texColor ;
			glm::vec3 lightDirection = light.Position - hitInfo.WorldIntersect;
			float distance = glm::length2(lightDirection);
			lightDirection = glm::normalize(lightDirection);
			glm::vec3 normal{ hitInfo.HitTriangle->GetNormal() };
			normal = glm::normalize(hitInfo.HitModel->transform.GetTransform() * glm::vec4{ normal ,0.f});
			float NdotL = glm::dot(normal, lightDirection);
			float intensity = glm::clamp(NdotL, 0.f, 1.f);
			currentLightColor *= intensity * light.Color * light.Intensity / distance;
			glm::vec3 half = glm::normalize(lightDirection + glm::normalize(rayOrigin - hitInfo.WorldIntersect));
			float NdotH = glm::dot(normal, half);
			intensity = glm::pow(glm::clamp(NdotH, 0.f, 1.f), 4);
			currentLightColor += (intensity * light.Color * light.Intensity / distance);
		}
		out += currentLightColor;
	}
	return out;
}
