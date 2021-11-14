#include "pch.h"
#include <Core/PathTracer.h>
#include <glm/vec4.hpp>

#include "Core/BVH/SceneBVH.h"
#include <Core/Scene.h>
#include <Core/EnvironmentMap.h>
RT::PathTracer::PathTracer(std::shared_ptr<Scene> sceneToRender):m_activeScene{sceneToRender}
{
}

void RT::PathTracer::ChangeSchene(std::shared_ptr<Scene> newSceneToRender)
{
	m_activeScene = newSceneToRender;
}

glm::vec3 RT::PathTracer::Trace(const Ray& ray, int currentDepth) const
{
	HitInfo hitInfo;
	hitInfo.Distance = INFINITY;
	hitInfo.Hit = false;
	RT::ReturnInfo returnInfo = m_activeScene->sceneBvh->Intersect(m_activeScene->models, ray, hitInfo.Distance);
	hitInfo.HitTriangle = returnInfo.triangle;
	if (hitInfo.HitTriangle)
	{
		hitInfo.Hit = true;
		glm::vec3 hitPoint = returnInfo.distance * ray.direction + ray.origin;
		hitInfo.LocalIntersect = returnInfo.model->transform.GetInverse() * glm::vec4{ hitPoint ,1.f };
		hitInfo.WorldIntersect = hitPoint;
		hitInfo.Distance = returnInfo.distance;
		hitInfo.HitTriangle = returnInfo.triangle;
		hitInfo.Material = &returnInfo.model->material;
		hitInfo.HitModel = returnInfo.model;

		glm::vec3 normal{ hitInfo.HitTriangle->GetNormal() };
		hitInfo.TransformedNormal = glm::normalize(hitInfo.HitModel->transform.GetTransform() * glm::vec4{ normal ,0.f });

	}
	if(hitInfo.Hit)
	{
		return  hitInfo.Material->baseColor;
	}
	return m_activeScene->environmentMap->Intersect(ray);
}

void RT::PathTracer::SetOptions(RT::RenderingOptions::OptionsData options)
{
}
