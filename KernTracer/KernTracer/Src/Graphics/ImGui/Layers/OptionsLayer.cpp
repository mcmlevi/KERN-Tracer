#include "pch.h"
#include <Graphics/ImGui/Layers/OptionsLayer.h>
#include <Graphics/ImGui/imgui.h>
#include <Core/RenderingOptions.h>
void RT::OptionsLayer::Update(float dt)
{

	ImGui::Begin("Options");
	int option = RenderingOptions::NOEFFECT;
	if(ImGui::RadioButton("None",&radioButton,eOptions::NONE))
	{
		option = RenderingOptions::NOEFFECT;
		m_setOptionsDelegate(std::move(option));
	}
	if (ImGui::RadioButton("Enable Normal View", &radioButton,eOptions::NORMALS))
	{
		option = RenderingOptions::NORMAL;
		m_setOptionsDelegate(std::move(option));
	}
	if (ImGui::RadioButton("Enable Depth View", &radioButton,eOptions::DEPTH))
	{
		option = RenderingOptions::DEPTH;
		m_setOptionsDelegate(std::move(option));
	}
	
	ImGui::End();
}
