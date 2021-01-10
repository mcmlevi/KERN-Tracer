#pragma once
#include <Graphics/ImGui/Layers/IImGuiLayer.h>

#include "Core/Scene.h"

namespace RT
{
	class StatLayer : public IImGuiLayer
	{
	public:
		void Update(float dt, std::shared_ptr<Scene> currentScene) override;
	private:
		float m_counter= 0.f;
		uint32_t m_frames = 0;
		float m_average = 0.f;
		float m_samplePeriod = 2000.f;
		
	};

}

