#include "pch.h"
#include <Graphics/ImGui/Layers/OptionsLayer.h>
#include <Graphics/ImGui/imgui.h>
#include <Core/Scene.h>
#include <Core/Camera.h>

RT::OptionsLayer::OptionsLayer(std::shared_ptr<RT::Scene> currentScene)
{
	m_scene = currentScene;
}

void RT::OptionsLayer::Update(float dt, std::shared_ptr<RT::Scene> currentScene)
{

	ImGui::Begin("Options");
	RenderingOptions::OptionsData option{};
	if(ImGui::RadioButton("None",&radioButton,eOptions::NONE))
	{
		m_currentMode = RenderingOptions::NOEFFECT;
	}
	if (ImGui::RadioButton("Enable Normal View", &radioButton,eOptions::NORMALS))
	{
		m_currentMode = RenderingOptions::NORMAL;
	}
	if (ImGui::RadioButton("Enable Depth View", &radioButton,eOptions::DEPTH))
	{
		m_currentMode = RenderingOptions::DEPTH;
	}

	if (ImGui::RadioButton("Enable BVH View", &radioButton, eOptions::SHOWBVH))
	{
		m_currentMode = RenderingOptions::VIEWBVH;
	}

	if (ImGui::RadioButton("Enable PixelCost View", &radioButton, eOptions::SHOWPIXELCOST))
	{
		m_currentMode = RenderingOptions::PIXELCOST;
	}
	if(radioButton == eOptions::SHOWBVH)
	{
		ImGui::InputInt("BVH View Depth", &m_drawDepth, 1, 5);
		if (m_scene)
		{
			m_scene->sceneBvh->DrawBVH(m_scene->activeCam->GetProjection(), m_scene->activeCam->GetView(), m_drawDepth);
		}
	}
	if (radioButton == eOptions::SHOWPIXELCOST)
	{
		ImGui::DragFloat("Pixel Cost Sensitivity", &m_sensitivity, 1, 0.f,1000.f);
		option.PixelCostSensitivity = m_sensitivity;
	}

	option.Mode = m_currentMode;
	m_setOptionsDelegate(std::move(option));
	ImGui::End();
}
