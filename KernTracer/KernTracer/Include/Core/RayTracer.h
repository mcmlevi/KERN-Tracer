#pragma once
#include <Core/Ray.h>
#include <memory>
#include <glm/vec3.hpp>
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
		void SetOptions(int32_t options);
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
		};
		
		glm::vec3 ApplyShading(const HitInfo& hitInfo, const Ray& rayOrigin, const glm::vec3& color) const;
		glm::vec3 Reflect(const HitInfo& hitInfo, const Ray& ray, const int depth) const;
		int m_maxDepth{ 3 };
		std::shared_ptr<Scene> m_activeScene{nullptr};
		int32_t m_options{0};
	};
}


