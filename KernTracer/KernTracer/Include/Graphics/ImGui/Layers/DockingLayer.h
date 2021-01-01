#pragma once
#include <Graphics/ImGui/Layers/IImGuiLayer.h>

namespace RT
{
	class DockingLayer : public IImGuiLayer
	{
	public:
		void Update(float dt) override;
	};
}


