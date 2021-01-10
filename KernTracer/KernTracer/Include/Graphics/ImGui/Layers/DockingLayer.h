#pragma once
#include <Graphics/ImGui/Layers/IImGuiLayer.h>

#include "Core/Scene.h"

namespace RT
{
	class DockingLayer : public IImGuiLayer
	{
	public:
		void Update(float dt, std::shared_ptr<Scene> currentScene) override;
	};
}


