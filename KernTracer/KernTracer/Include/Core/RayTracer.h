#pragma once
#include <core/Ray.h>
namespace RT
{
	class Scene;
	class Triangle;
	class Camera;
	
	class RayTracer
	{
	public:
		RayTracer(std::shared_ptr<Scene> sceneToRender);
		void ChangeSchene(std::shared_ptr<Scene> newSceneToRender);
		glm::vec3 Trace(const Ray& ray, int currentDepth) const;
	private:

		glm::vec3 ApplyShading(const glm::vec3& hitPoint, const RT::Triangle* triangle, const glm::vec3& rayOrigin) const;
		int m_maxDepth{ 3 };
		std::shared_ptr<Scene> m_activeScene{nullptr};
	};
}


