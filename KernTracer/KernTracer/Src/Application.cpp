#include <pch.h>
#include <Application.h>

#include <Core/Scene.h>
#include "Core/ResourceManager.h"
#include <Core/Camera.h>
#include <Core/PointLight.h>

#include "Core/JobSystem.h"

#include <Graphics/Window/Window.h>

#include "Core/Timer.h"
#include "Graphics/OpenGL/glad.h"
#include <Graphics/ImGui/ImGuiInstance.h>
#include <Graphics/ImGui/Layers/DockingLayer.h>
#include <Graphics/ImGui/Layers/StatLayer.h>

#include "Graphics/ImGui/Layers/OptionsLayer.h"
#include <Core/EnvironmentMap.h>
#include <Core/Triangle.h>
#include <Core/BVH/BVH.h>
#include <glm/gtc/random.hpp>




void RenderJob(const std::shared_ptr<RT::TRACER>& tracer, const glm::vec2 pos, const glm::vec2 chunkSize, const glm::vec2 winsize, const std::shared_ptr<RT::Camera>& cam, glm::vec3* buf)
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
	m_testScene = std::make_shared<RT::Scene>();
	m_tracer = std::make_shared<TRACER>(m_testScene);

	m_imGuiInstance = std::make_unique<ImGuiInstance>(m_window);
    m_imGuiInstance->AddLayer(std::make_unique<DockingLayer>());
    m_imGuiInstance->AddLayer(std::make_unique<StatLayer>());

    std::unique_ptr<OptionsLayer> optionsLayer = std::make_unique<OptionsLayer>(m_testScene);
    optionsLayer->m_setOptionsDelegate.Connect(&RT::TRACER::SetOptions, m_tracer.get());
	m_imGuiInstance->AddLayer(std::move(optionsLayer));
	
	m_resourceManager = std::make_shared<RT::ResourceManager>();
	m_CurrentCamera = std::make_shared<RT::Camera>(RT::Camera{ 70, m_window->GetSize(), {0.f,0.f,-10.f }, { 0,0,0 } });

	
	m_testScene->activeCam = m_CurrentCamera;
	std::shared_ptr<RT::ResourceManager> resourceManager{ std::make_shared<RT::ResourceManager>() };
	m_testScene->pointLights.push_back(std::make_shared<RT::PointLight>(RT::PointLight{ { 0,20,0 }, { 1.f,1.f,1.f }, 300.f }));
   // m_testScene->pointLights.push_back(std::make_shared<RT::PointLight>(RT::PointLight{ { 0,0,100.f }, { 1.f,1.f,1.f }, 1000.f }));
    m_testScene->environmentMap = std::make_unique<RT::EnvironmentMap>("Assets/old_hall_16k.hdr");
	//for (int i = 0; i < 30; ++i)
	//{
 //       auto model = std::make_shared<RT::Model>();
 //       //model->modelData = resourceManager->LoadModel("Assets/Potato/potato.obj");
 //      model->modelData = resourceManager->LoadModel("Assets/dragon_100k.obj");
 //       model->material = *model->modelData->loadedInMaterial;
 //       model->material.baseColor = { 1.f,1.f,1.f };
 //       model->material.reflectiveIndex = 0.5;
 //       //model->material.refractiveIndex = 0.5;
	//	model->transform.SetPosition( glm::ballRand(10.f));
 //       model->transform.SetRotation(glm::vec3{glm::linearRand(0,360),glm::linearRand(0,360) ,glm::linearRand(0,360) });
 //       //model->transform.SetScale({ 100.f,100.f,100.ff });
 //       m_testScene->models.push_back(model);
	//}
    for (int x = 0; x < 10; ++x)
    {
	    for (int y = 0; y < 10; ++y)
	    {
            auto model = std::make_shared<RT::Model>();
          //  model->modelData = resourceManager->LoadModel("Assets/Potato/potato.obj");
            model->modelData = resourceManager->LoadModel("Assets/dragon_100k.obj");
            model->material = *model->modelData->loadedInMaterial;
            model->material.baseColor = { 1.f,1.f,1.f };
            model->material.reflectiveIndex = 0.5;
            model->material.refractiveIndex = 1.5;
            model->transform.SetPosition({ x - 5.f,y-5.f,0.f });
          //  model->transform.SetRotation(glm::vec3{glm::linearRand(0,360),glm::linearRand(0,360) ,glm::linearRand(0,360) });
          //  model->transform.SetScale({ 100.f,100.f,100.f });
            m_testScene->models.push_back(model);
	    }
       
    }
    //auto plane = std::make_shared<RT::Model>();
    //plane->modelData = resourceManager->LoadModel("Assets/cube.obj");
    //plane->material = *plane->modelData->loadedInMaterial;
    //plane->material.baseColor = { 1.f,1.f,1.f };
    //plane->transform.Scale({ 100.0f,0.01f,100.f });
    //plane->transform.SetPosition({ 0.f,-6.f,0.f });
    //m_testScene->models.push_back(plane);
	
 //   auto model2 = std::make_shared<RT::Model>();
 //   model2->modelData = resourceManager->LoadModel("Assets/Potato/potato.obj");
 //   model2->material = *model2->modelData->loadedInMaterial;
 //   model2->material.baseColor = { 1.f,1.f,1.f };
 //   model2->material.refractiveIndex = 1.5f;
 //   model2->transform.SetPosition({ 0, 0, -15.f });
 //   model2->transform.SetScale({ 3, 3, 3.f });
 //   model2->transform.SetRotation({ 0,45,0 });
	//m_testScene->models.push_back(model2);
	
	
	m_tracer->ChangeSchene(m_testScene);
 //   cow1 = std::make_shared<RT::Model>();
 //   cow1->modelData = resourceManager->LoadModel("Assets/highresSphere.obj");
 //   cow1->material = *resourceManager->GetMaterial("Default");
	////std::shared_ptr<RT::Material> redMaterial{ resourceManager->GetMaterial("RedMaterial") };
	////redMaterial->baseColor = { 1.f,1.f,1.f };
	////cow1->material = *cow1->modelData->loadedInMaterial;
	//cow1->material.baseColor = { 0.f,0.f,1.f };
 //   //cow1->material.reflectiveIndex = 0.4f;
 //   cow1->material.refractiveIndex = 1.25f;
 //   //cow1->transform.Scale({ 2.0f,2.0f,2.0f });
 //   cow1->transform.Scale({ 0.25f,0.25f,0.25f });
 //   cow1->transform.SetPosition({ 0,0,0 });
 //   m_testScene->models.push_back(cow1);
 //   m_testScene->sceneBvh->BuildBVH(m_testScene->models);
	Tick();
}

void RT::Application::Tick()
{
    while (!m_window->ShouldClose())
    {
        RT::Timer timer{};
		m_testScene->sceneBvh->BuildBVH(m_testScene->models);
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



        m_imGuiInstance->Update(m_deltaTime,m_testScene);
        m_imGuiInstance->Render();

      

        //cow1->transform.Rotate({ 0.f, 0.001f * m_deltaTime, 0.f });

       // m_testScene->sceneBvh->DrawBVH(m_CurrentCamera->GetProjection(), m_CurrentCamera->GetView(),1);
    	
        m_window->Update();
        m_testScene->activeCam->UpdateCamera(m_window->GetSize());
        m_deltaTime = timer.GetElapsedTimeInMS();
    }
}
