#include "pch.h"
#include <Graphics/ImGui/Layers/StatLayer.h>
#include <Graphics/ImGui/imgui.h>
void RT::StatLayer::Update(float dt)
{
	m_counter += dt;
	++m_frames;

	ImGui::Begin("StatWindow");

	std::string fps{ "FPS: " + std::to_string(1000.f / dt) + "\n" };
	ImGui::Text(fps.c_str());
	std::string tps{ "Time Per Frame: " + std::to_string(dt) + "\n" };
	ImGui::Text(tps.c_str());

	std::string avrfps{ "Average FPS: " + std::to_string(1000.f / m_average) + "\n" };
	ImGui::Text(avrfps.c_str());
	std::string avr{ "Average Time Per Frame: " + std::to_string(m_average) + "\n" };\
	ImGui::Text(avr.c_str());
	ImGui::End();


	
	if (m_counter >= m_samplePeriod)
	{
		m_average = m_counter / static_cast<float>(m_frames);
		m_frames = 0;
		m_counter = 0;
	}
}
