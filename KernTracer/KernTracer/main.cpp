#include <pch.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/OpenGL/glad.h>
#include <Graphics/Window/Window.h>


int main(void)
{
    RT::Window window{1600,800,"Ray Tracer"};

    float* pixels{ (float*)malloc(sizeof(float) * 1600 * 800 * 3) };

    for (int i = 0; i < 1600 * 800 * 3; i += 3)
    {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        pixels[i] = r;
        pixels[i + 1] = 0.f;
        pixels[i + 2] = 0.f;
    }
	
    while (!window.ShouldClose())
    {

 
    	
        
		window.DrawPixelBuffer(window.GetSize(), GL_RGB, GL_FLOAT, (void*)pixels);
        window.Update();
    }
 
    delete[] pixels;
    exit(EXIT_SUCCESS);
}