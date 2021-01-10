#pragma once
#include <memory>
#include <vector>

#include "Core/Scene.h"

struct ImGuiIO;

namespace RT
{
	class IImGuiLayer;
	class Window;
	class ImGuiInstance
	{
	public:
		ImGuiInstance(std::shared_ptr<Window>& window);
		~ImGuiInstance();
		void AddLayer(std::unique_ptr<IImGuiLayer>&& layer);
		void Update(float deltaTime, std::shared_ptr<Scene> currentScene);
		void Render();

	private:
		std::vector<std::unique_ptr<IImGuiLayer>> m_layers;
		ImGuiIO* m_io;
	};
}


