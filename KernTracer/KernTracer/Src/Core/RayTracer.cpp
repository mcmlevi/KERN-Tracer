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
#include <Core/EnvironmentMap.h>
#include "Core/Timer.h"

RT::RayTracer::RayTracer(std::shared_ptr<Scene> sceneToRender)
{
	m_activeScene = sceneToRender;
}

void RT::RayTracer::ChangeSchene(std::shared_ptr<Scene> newSceneToRender)
{
	m_activeScene = newSceneToRender;
}
thread_local RT::Timer pixelTimer;

glm::vec3 RT::RayTracer::Trace(const Ray& ray, int currentDepth) const
{
	if(m_options.Mode & RT::RenderingOptions::PIXELCOST && currentDepth == 0)
	{
		pixelTimer.ResetTimer();
	}

	if (currentDepth >= m_maxDepth)
		return {};
	
	HitInfo hitInfo;
	hitInfo.Distance = INFINITY;
	hitInfo.Hit = false;
	RT::ReturnInfo returnInfo = m_activeScene->sceneBvh->Intersect(m_activeScene->models, ray, hitInfo.Distance);
	hitInfo.HitTriangle = returnInfo.triangle;
	if(hitInfo.HitTriangle)
	{
		hitInfo.Hit = true;
		glm::vec3 hitPoint = returnInfo.distance * ray.direction + ray.origin;
		hitInfo.LocalIntersect = returnInfo.model->transform.GetInverse() * glm::vec4{ hitPoint ,1.f};
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
		if(m_options.Mode & RenderingOptions::NORMAL)
		{
			return hitInfo.TransformedNormal;
		}
		else if(m_options.Mode & RenderingOptions::DEPTH)
		{
			return (glm::vec3{0.f,0.f,1.f} / sqrtf(hitInfo.Distance));
		}
		//return { 1.f,0.f,0.f };

		glm::vec3 texColor{ hitInfo.Material->GetTexture(hitInfo.LocalIntersect,*hitInfo.HitTriangle) };

		float fresnel{ 0.f };
		glm::vec3 reflectionResult(calculateRefraction(ray,hitInfo, fresnel, currentDepth));

		//texColor  (hitInfo.Material->baseColor * reflectionResult);
		if (hitInfo.Material->refractiveIndex == 0.f)
		{
			glm::vec3 shadingResult{ ApplyShading(hitInfo, ray,texColor) };
			if (currentDepth == 0 && m_options.Mode & RT::RenderingOptions::PIXELCOST)
				return CalculatePixelCost();
			return shadingResult + reflectionResult;
		}
		if (currentDepth == 0 && m_options.Mode & RT::RenderingOptions::PIXELCOST)
			return CalculatePixelCost();
		return reflectionResult;
		
			
	}
	if (currentDepth == 0 && m_options.Mode & RT::RenderingOptions::PIXELCOST)
		return CalculatePixelCost();
	return m_activeScene->environmentMap->Intersect(ray);
}

void RT::RayTracer::SetOptions(RenderingOptions::OptionsData options)
{
	m_options = options;
}

float RT::RayTracer::Calculatefresnel(const RT::Ray& ray, const glm::vec3& normal, float refractionIndex) const
{
	float cosi = glm::clamp(-1.f, 1.f, glm::dot(ray.direction,normal));
	float etai = 1.f, etat = refractionIndex;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1.f - cosi * cosi));
	// Total internal reflection
	if (sint >= 1.f) {
		return 1.f;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1.f - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return  (Rs * Rs + Rp * Rp) * 0.5f;
	}
}

glm::vec3 RT::RayTracer::calculateRefraction(const Ray& ray, const HitInfo& info,float& fresnel, int depth) const
{

	glm::vec3 RefractionColor{};
	glm::vec3 ReflectionColor{};
	if (info.Material->refractiveIndex > 0.f)
	{
		fresnel = Calculatefresnel(ray, info.TransformedNormal, info.Material->refractiveIndex);
		bool outside{ glm::dot(ray.direction,info.TransformedNormal) < 0 };
		if (fresnel < 1.f)
		{
			// get the transmission Ray
			 glm::vec3 TransmissionRay{refract(ray,info) };
			//glm::vec3 TransmissionRay{glm::refract(ray.direction,info.TransformedNormal,info.Material->refractiveIndex) };
				TransmissionRay = glm::normalize(TransmissionRay);


				// Make the actual refraction Ray
				Ray refractionRay{  outside ? info.WorldIntersect - info.TransformedNormal * 1e-4f : info.WorldIntersect + info.TransformedNormal * 1e-4f,TransmissionRay };

				RefractionColor = Trace(refractionRay, depth + 1);
		
		}
		glm::vec3 reflectionDirection = glm::normalize(glm::reflect(ray.direction, info.TransformedNormal));
		glm::vec3 reflectionRayOrig{ outside ? info.WorldIntersect - info.TransformedNormal * 1e-4f : info.WorldIntersect + info.TransformedNormal * 1e-4f };
		ReflectionColor = Trace(Ray{ reflectionRayOrig,reflectionDirection }, depth + 1);

		return ReflectionColor * fresnel + RefractionColor * (1.f - fresnel);
	}
	return  { 0.f,0.f,0.f };
}

glm::vec3 RT::RayTracer::refract(const Ray& ray, const HitInfo& info) const
{
	float cosi = glm::clamp(-1.f, 1.f, glm::dot(ray.direction,info.TransformedNormal));
	float etai = 1.f, etat = info.Material->refractiveIndex;
	glm::vec3 n = info.TransformedNormal;
	if (cosi < 0) { cosi = -cosi; }
	else
	{
		std::swap(etai, etat);
		n = -1.f * n;
	}
	float eta = etai / etat;
	float k = 1.f - eta * eta * (1.f - cosi * cosi);
	return k < 0.f ? glm::vec3{} : eta * ray.direction + (eta * cosi - sqrtf(k)) * n;
}
int interpolate(int color1, int color2, float fraction)
{
	unsigned char   r1 = (color1 >> 16) & 0xff;
	unsigned char   r2 = (color2 >> 16) & 0xff;
	unsigned char   g1 = (color1 >> 8) & 0xff;
	unsigned char   g2 = (color2 >> 8) & 0xff;
	unsigned char   b1 = color1 & 0xff;
	unsigned char   b2 = color2 & 0xff;

	return (int)((r2 - r1) * fraction + r1) << 16 |
		(int)((g2 - g1) * fraction + g1) << 8 |
		(int)((b2 - b1) * fraction + b1);
}

glm::vec3 RT::RayTracer::CalculatePixelCost() const
{
	float time = pixelTimer.GetElapsedTimeInMS();
	int result = interpolate(0x00FF00,0xFF0000,time * m_options.PixelCostSensitivity);
	unsigned char   r = (result >> 16) & 0xff;
	unsigned char   g = (result >> 8) & 0xff;
	unsigned char   b = result & 0xff;
	return { r / 255.f,g / 255.f,b / 255.f };
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
			glm::vec3 normal{ hitInfo.TransformedNormal };
			lightVisable = m_activeScene->sceneBvh->LightTraverse(light.Position, m_activeScene->models, ray);
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
		glm::vec3 normal{ hitInfo.TransformedNormal };
		glm::vec3 reflectDir = glm::reflect(ray.direction, normal);
		Ray reflectiveRay{ hitInfo.WorldIntersect + normal * 1e-4f,reflectDir };
		return Trace(reflectiveRay, depth + 1) * hitInfo.Material->reflectiveIndex;
	}
	return { 0.f,0.f,0.f };
}
