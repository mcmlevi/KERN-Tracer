#pragma once
#include <cstdint>
#include <string>
#include <glm/vec2.hpp>

struct GLFWwindow;
namespace RT
{
	class Window
	{
	public:
		Window(unsigned int width, unsigned int height, const std::string& windowName);
		Window(const glm::vec2& size, const std::string& windowName);
		~Window();
		void Update();
		[[nodiscard]] bool ShouldClose() const;
		[[nodiscard]] glm::vec<2,unsigned int> GetSize() const;

		void DrawPixelBuffer(unsigned int width, unsigned int height, unsigned int format, unsigned int type, const void* pixelBuf) const;
		void DrawPixelBuffer(glm::vec<2,unsigned int> size, unsigned int format, unsigned int type, const void* pixelBuf) const;
	private:
		void InitWindow();
		
		glm::vec2 m_WindowSize;
		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}


