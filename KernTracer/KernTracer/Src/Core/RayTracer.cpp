#include "pch.h"
#include <Core/RayTracer.h>
#include <Core/Triangle.h>
#include <Core/ObjLoader.h>
#include <Core/PointLight.h>
#include <glm/gtx/norm.hpp>
#include <Core/BVH/BVH.h>
#include <Core/Scene.h>
#include <Core/Material.h>
#include <Core/RenderingOptions.h>
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
		if(m_options & RenderingOptions::NORMAL)
		{
			glm::vec3 normal{ hitInfo.HitTriangle->GetNormal() };
			return glm::normalize(hitInfo.HitModel->transform.GetTransform() * glm::vec4{ normal ,0.f });
		}
		else if(m_options & RenderingOptions::DEPTH)
		{
			return (glm::vec3{0.f,0.f,1.f} / sqrtf(hitInfo.Distance));
		}
		//return { 1.f,0.f,0.f };
		glm::vec3 reflectionResult(Reflect(hitInfo, ray, currentDepth));

		glm::vec3 texColor{ hitInfo.Material->GetTexture(hitInfo.LocalIntersect,*hitInfo.HitTriangle) };
		//texColor  (hitInfo.Material->baseColor * reflectionResult);
		glm::vec3 shadingResult{ ApplyShading(hitInfo, ray,texColor) * (1.f-hitInfo.Material->reflectiveIndex) };

		return shadingResult + reflectionResult;
			
	}
	return { 0.f,1.f,0.f };
}


void RT::RayTracer::SetOptions(int32_t options)
{
	m_options = options;
}

glm::vec3 RT::RayTracer::ApplyShading(const HitInfo& hitInfo, const Ray& ray, const glm::vec3& color) const
{
	glm::vec3 out{};
	for (size_t i = 0; i < m_activeScene->pointLights.size(); ++i)
	{
		glm::vec3 currentLightColor{};
		const RT::PointLight& light = *m_activeScene->pointLights[i];
		if (light.Intensity > 0)
		{

			bool lightVisable = true;
			glm::vec3 normal{ hitInfo.HitTriangle->GetNormal() };
			normal = glm::normalize(hitInfo.HitModel->transform.GetTransform() * glm::vec4{ normal ,0.f });
			for (size_t i = 0; i < m_activeScene->models.size(); ++i)
			{
				const auto& modelref = m_activeScene->models[i];
				glm::vec3 toLight{ glm::normalize(light.Position - hitInfo.WorldIntersect) };
				glm::vec3 trd = glm::normalize(modelref->transform.GetInverse() * glm::vec4{ toLight,0.f });
				glm::vec3 tro = modelref->transform.GetInverse() * glm::vec4{ hitInfo.WorldIntersect + normal * 1e-4f,1.f };
				Ray transformedRay{ tro,trd };
				bool hit{ m_activeScene->models[i]->modelData->bvh->LightTraverse(light.Position,*m_activeScene->models[i], transformedRay) };
				if (hit)
				{
					lightVisable = false;
					break;
				}
			}
			if(lightVisable)
			{
				glm::vec3 lightDirection = light.Position - hitInfo.WorldIntersect;
				float distance = glm::length2(lightDirection);
				lightDirection = glm::normalize(lightDirection);
				
				float NdotL = glm::dot(normal, lightDirection);
				float intensity = glm::clamp(NdotL, 0.f, 1.f);
				currentLightColor *= intensity * light.Color * light.Intensity / distance;
				glm::vec3 half = glm::normalize(lightDirection + glm::normalize(ray.origin - hitInfo.WorldIntersect));
				float NdotH = glm::dot(normal, half);
				intensity = glm::pow(glm::clamp(NdotH, 0.f, 1.f), 4);
				currentLightColor += color * (intensity * light.Color * light.Intensity / distance);
			}
		}
		out += currentLightColor;
	}
	return out;
}

glm::vec3 RT::RayTracer::Reflect(const HitInfo& hitInfo, const Ray& ray, const int depth) const
{
	if(hitInfo.Material->reflectiveIndex > 0.f)
	{
		glm::vec3 normal{ glm::normalize(hitInfo.HitModel->transform.GetTransform() * glm::vec4{ hitInfo.HitTriangle->GetNormal() ,0.f}) };
		glm::vec3 reflectDir = glm::reflect(ray.direction, normal);
		Ray reflectiveRay{ hitInfo.WorldIntersect + normal * 1e-4f,reflectDir };
		return Trace(reflectiveRay, depth + 1) * hitInfo.Material->reflectiveIndex;
	}
	return { 0.f,0.f,0.f };
}
