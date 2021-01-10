#pragma once
#include <Core/Scene.h>
namespace RT
{
	class IImGuiLayer
	{
	public:
		IImGuiLayer() = default;
		virtual ~IImGuiLayer() = default;
		virtual void Update(float dt, std::shared_ptr<Scene> currentScene) = 0;
		
	};
}
