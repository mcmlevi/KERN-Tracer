#include <pch.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/OpenGL/glad.h>
#include <Graphics/Window/Window.h>
#include <Core/Camera.h>
#include <Core/RayTracer.h>
int main(void)
{
    RT::Window window{1600,800,"Ray Tracer"};

    glm::vec3* pixels{ (glm::vec3*)malloc(sizeof(glm::vec3) * window.GetSize().x * window.GetSize().y) };
    RT::Camera cam{ 70, window.GetSize(), { 0,0,15 }, { 0,0,-5 } };
    RT::RayTracer tracer{};
    while (!window.ShouldClose())
    {
        int ptr{};
        for (int y = 0; y < window.GetSize().y; ++y)
        {
            for (int x = 0; x < window.GetSize().x; ++x)
            {
                pixels[ptr] = tracer.Trace(cam.GetRayAt({ x,y }), 0);
                ++ptr;
            }
        }
		window.DrawPixelBuffer(window.GetSize(), GL_RGB, GL_FLOAT, (void*)pixels);
        window.Update();
    }
 
    delete[] pixels;
    exit(EXIT_SUCCESS);
}