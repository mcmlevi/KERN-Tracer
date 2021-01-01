#include <pch.h>
#include <Application.h>

#include <Core/Scene.h>
#include "Core/ResourceManager.h"
#include <Core/Camera.h>
#include <Core/PointLight.h>
#include <Core/RayTracer.h>
#include "Core/JobSystem.h"

#include <Graphics/Window/Window.h>

#include "Core/Timer.h"
#include "Graphics/OpenGL/glad.h"
#include <Graphics/ImGui/ImGuiInstance.h>
#include <Graphics/ImGui/Layers/DockingLayer.h>
#include <Graphics/ImGui/Layers/StatLayer.h>

void RenderJob(const std::shared_ptr<RT::RayTracer>& tracer, const glm::vec2 pos, const glm::vec2 chunkSize, const glm::vec2 winsize, const std::shared_ptr<RT::Camera>& cam, glm::vec3* buf)
{
    for (int y = 0; y < chunkSize.y; y++)
    {
        for (int x = 0; x < chunkSize.x; x++)
        {
            buf[static_cast<int>(((pos.y + y) * winsize.x + pos.x + x))] = tracer->Trace(cam->GetRayAt({ pos.x + x,pos.y + y }), 0);
        }
    }
}

std::shared_ptr<RT::Model> cow1;
void RT::Application::Initialize()
{
	JobSystem::Initialize();
	
	m_window = std::make_shared<Window>(800, 800, "Ray Tracer");

	m_imGuiInstance = std::make_unique<ImGuiInstance>(m_window);
    m_imGuiInstance->AddLayer(std::make_unique<DockingLayer>());
    m_imGuiInstance->AddLayer(std::make_unique<StatLayer>());
	
	m_testScene = std::make_shared<RT::Scene>();
	m_tracer = std::make_shared<RayTracer>(m_testScene);
	m_resourceManager = std::make_shared<RT::ResourceManager>();
	m_CurrentCamera = std::make_shared<RT::Camera>(RT::Camera{ 70, m_window->GetSize(), {0,3.f,15.f }, { 0,0,0 } });

	
	m_testScene->activeCam = m_CurrentCamera;
	std::shared_ptr<RT::ResourceManager> resourceManager{ std::make_shared<RT::ResourceManager>() };
	m_testScene->pointLights.push_back(std::make_shared<RT::PointLight>(RT::PointLight{ { 0,10,8 }, { 1.f,1.f,1.f }, 50.f }));
	cow1 = std::make_shared<RT::Model>();
	cow1->modelData = resourceManager->LoadModel("Assets/Potato/potato.obj");
	cow1->material = *resourceManager->GetMaterial("Default");
	m_testScene->models.push_back(cow1);

	std::shared_ptr<RT::Material> redMaterial{ resourceManager->GetMaterial("RedMaterial") };
	redMaterial->baseColor = { 1.f,1.f,1.f };
	cow1->material = *cow1->modelData->loadedInMaterial;
	cow1->material.baseColor = { 1.f,1.f,1.f };

    Tick();
}

void RT::Application::Tick()
{
    while (!m_window->ShouldClose())
    {
        RT::Timer timer{};
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const glm::vec2 winSize = m_window->GetSize();
        for (unsigned y = 0; y < m_window->GetSize().y; y += m_chunkSize)
        {
            for (int x = 0; x < winSize.x; x += m_chunkSize)
            {
                glm::vec2 chunk{ m_chunkSize,m_chunkSize };
                if (y + m_chunkSize >= winSize.y)
                {
                    chunk.y = winSize.y - y;
                }
                if (x + m_chunkSize >= winSize.x)
                {
                    chunk.x = winSize.x - x;
                }
                glm::vec2 pos{ x,y };
                JobSystem::Execute([pos, chunk, winSize,this]()
                {
	                RenderJob(m_tracer, pos, chunk, winSize, m_CurrentCamera, m_window->GetScreenBuffer());
                }, &m_counter);
            }
        }
#pragma warning(push,0)
#pragma warning(disable: 28020)
        JobSystem::Wait(&m_counter);
#pragma warning(pop)
        m_window->DrawPixelBuffer(m_window->GetSize(), GL_RGB, GL_FLOAT, (void*)m_window->GetScreenBuffer());

		cow1->transform.Rotate(glm::vec3{ 0.f,1.f,0.f } *m_deltaTime * 0.001f);


        m_imGuiInstance->Update(m_deltaTime);
        m_imGuiInstance->Render();
    
        m_window->Update();
        m_deltaTime = timer.GetElapsedTimeInMS();
    }
}
