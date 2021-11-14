#pragma once
#include <Core/RenderingOptions.h>
namespace RT
{
	struct Material;
	struct Scene;
	struct Ray;
	struct Model;
	class Triangle;
	class PathTracer
	{
	public:
		PathTracer(std::shared_ptr<Scene> sceneToRender);
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
		
		int m_maxDepth{ 5 };
		std::shared_ptr<Scene> m_activeScene{ nullptr };
		RT::RenderingOptions::OptionsData m_options{ 0 };
	};
}


