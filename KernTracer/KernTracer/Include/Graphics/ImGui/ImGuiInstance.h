#pragma once
#include <memory>
#include <vector>

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
		void Update(float deltaTime);
		void Render();

	private:
		std::vector<std::unique_ptr<IImGuiLayer>> m_layers;
		ImGuiIO* m_io;
	};
}


