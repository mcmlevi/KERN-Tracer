#pragma once
#include <Core/Ray.h>
#include <memory>
#include <glm/vec3.hpp>
#include <Core/RenderingOptions.h>

namespace RT
{
	struct Scene;
	class Triangle;
	class Camera;
	struct Material;
	struct Model;
	class RayTracer
	{
	public:
		RayTracer(std::shared_ptr<Scene> sceneToRender);
		void ChangeSchene(std::shared_ptr<Scene> newSceneToRender);
		glm::vec3 Trace(const Ray& ray, int currentDepth) const;
		void SetOptions(RT::RenderingOptions::OptionsData options);
	private:
		struct HitInfo
		{
			bool Hit;
			float Distance;
			const Triangle* HitTriangle;
			std::shared_ptr<RT::Model> HitModel;
			const RT::Material* Material;
			glm::vec3 WorldIntersect;
			glm::vec3 LocalIntersect;
			glm::vec3 TransformedNormal;
		};
		
		glm::vec3 ApplyShading(const HitInfo& hitInfo, const Ray& rayOrigin, const glm::vec3& color) const;
		glm::vec3 Reflect(const HitInfo& hitInfo, const Ray& ray, const int depth) const;
		float Calculatefresnel(const RT::Ray& ray, const glm::vec3& normal, float refractionIndex) const;
		glm::vec3 calculateRefraction(const Ray& ray, const HitInfo& info, float& fresnel, int depth) const ;
		glm::vec3 refract(const Ray& ray, const HitInfo& info) const;
		glm::vec3 CalculatePixelCost() const;
		int m_maxDepth{ 3 };
		std::shared_ptr<Scene> m_activeScene{nullptr};
		RT::RenderingOptions::OptionsData m_options{0};
		
	};
}


