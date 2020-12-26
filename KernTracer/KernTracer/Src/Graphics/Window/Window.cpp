#include <pch.h>
#include "Graphics/Window/Window.h"

#include <Graphics/OpenGL/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


static void error_callback(int error, const char* description)
{
    (void)error;
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)mods;
    (void)scancode;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}




RT::Window::Window(unsigned int width, unsigned int height, const std::string& windowName):
m_WindowSize{width,height},m_WindowName{windowName}
{
    InitWindow();
}

RT::Window::Window(const glm::vec2& size, const std::string& windowName):
m_WindowSize{size},m_WindowName{windowName}
{
    InitWindow();
}

RT::Window::~Window()
{
    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void RT::Window::Update()
{
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

[[nodiscard]] bool RT::Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

[[nodiscard]] glm::vec<2, unsigned int> RT::Window::GetSize() const
{
    return m_WindowSize;
}

void RT::Window::DrawPixelBuffer(unsigned int width, unsigned int height, unsigned int format, unsigned int type,
    const void* pixelBuf) const
{
    glDrawPixels(width, height, format, type, pixelBuf);
}

void RT::Window::DrawPixelBuffer(glm::vec<2, unsigned> size, unsigned format, unsigned type,
	const void* pixelBuf) const
{
    DrawPixelBuffer(size.x, size.y, format, type, pixelBuf);
}

void RT::Window::InitWindow()
{

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_Window = glfwCreateWindow(static_cast<int>(m_WindowSize.x), static_cast<int>(m_WindowSize.y), m_WindowName.c_str(), NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(m_Window, key_callback);

    glfwMakeContextCurrent(m_Window);
    gladLoadGL();
    glfwSwapInterval(0);
}
