#pragma once
#include <memory>
#include <atomic>
#include <Graphics/ImGui/ImGuiInstance.h>

#define WHITTED
//#define PATH
#ifdef WHITTED
#include <Core/RayTracer.h>
#define TRACER RayTracer
#elif defined PATH
#include <Core/PathTracer.h>
#define TRACER PathTracer
#endif

namespace RT
{
	struct Scene;
	class ResourceManager;
	class Camera;
	class Window;
	class RayTracer;
	class ImGuiInstance;
	
	class Application
	{
	public:
		Application() = default;
		void Initialize();

	private:
		void Tick();
		std::shared_ptr<Scene>m_testScene;
		std::shared_ptr<ResourceManager> m_resourceManager;
		std::shared_ptr<RT::Camera> m_CurrentCamera;
		std::shared_ptr<Window> m_window;
		std::shared_ptr<TRACER> m_tracer;
		std::unique_ptr<ImGuiInstance> m_imGuiInstance;
		float m_deltaTime;
		uint32_t m_chunkSize = 16;
		std::atomic<int> m_counter;
	};
}


