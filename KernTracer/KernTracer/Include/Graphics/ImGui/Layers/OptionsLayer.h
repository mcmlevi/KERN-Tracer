#pragma once
#include <Graphics/ImGui/Layers/IImGuiLayer.h>
#include <signals/signals/Delegate.h>
#include <Core/RenderingOptions.h>
enum eOptions
{
	NONE = 0,
	NORMALS,
	DEPTH,
	SHOWBVH,
	SHOWPIXELCOST
};
namespace RT
{
	class Scene;
	class OptionsLayer : public IImGuiLayer
	{
	public:
		OptionsLayer(std::shared_ptr<RT::Scene> currentScene);
		void Update(float dt, std::shared_ptr<Scene> currentScene) override;
		SIG::Delegate<RT::RenderingOptions::OptionsData> m_setOptionsDelegate{};
	private:
		bool m_showNormals = false;
		int m_currentMode = 0;
		int radioButton = 0;
		int m_drawDepth = 0;
		float m_sensitivity = 1.f;
		std::shared_ptr<RT::Scene> m_scene;
	};
	
}


