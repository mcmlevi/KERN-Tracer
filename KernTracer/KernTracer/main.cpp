#include <pch.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/OpenGL/glad.h>
#include <Graphics/Window/Window.h>
#include <Core/Camera.h>
#include <Core/RayTracer.h>
#include <Core/JobSystem.h>
#include <Core/Timer.h>

#define CHUNKSIZE 8
#define FRAMES 1
void RenderJob(const RT::RayTracer& tracer, const glm::vec2 pos, const glm::vec2 chunkSize,const glm::vec2 winsize, const RT::Camera& cam, glm::vec3* buf)
{
    for (int y = 0; y < chunkSize.y; y++)
    {
        for (int x = 0; x < chunkSize.x; x++)
        {
            buf[static_cast<int>(((pos.y + y) * winsize.x + pos.x + x))] = tracer.Trace(cam.GetRayAt({ pos.x + x,pos.y + y }), 0);
        }
    }
}
void Render(std::atomic<int>* counter, int& frame, RT::Window& window, glm::vec3* buffer)
{
    JobSystem::Wait(counter);
	window.DrawPixelBuffer(window.GetSize(), GL_RGB, GL_FLOAT, (void*)buffer);
    window.Update();
    frame++;
    if (frame >= FRAMES)
    {
        frame = 0;
    }
}

int main(void)
{
    RT::Window window{800 ,800,"Ray Tracer"};
    
    std::array<glm::vec3*, FRAMES> buffers{};
    std::array<std::atomic<int>, FRAMES> counters;
    for (int i = 0; i < FRAMES; ++i)
    {
        buffers[i] = { (glm::vec3*)malloc(sizeof(glm::vec3) * window.GetSize().x * window.GetSize().y) };
        counters[i] = 0;
    }
    
	
    RT::Camera cam{ 70, window.GetSize(), { 0,0,10 }, { 0,0,-5 } };
    RT::RayTracer tracer{};
    JobSystem::Initialize();
    int currentBuffer{ 0 };
    while (!window.ShouldClose())
    {
        RT::Timer timer{};
        const glm::vec2 winSize = window.GetSize();
    	for (int y = 0; y < window.GetSize().y; y+= CHUNKSIZE)
        {
            for (int x = 0; x < winSize.x; x+= CHUNKSIZE)
            {
                glm::vec2 chunk{ CHUNKSIZE,CHUNKSIZE };
                if(y + CHUNKSIZE >= winSize.y)
                {
                    chunk.y = winSize.y - y;
                }
            	if(x + CHUNKSIZE >= winSize.x)
            	{
                    chunk.x = winSize.x - x;
            	}
            	glm::vec2 pos{ x,y };
                JobSystem::Execute([&tracer,pos,chunk,winSize,&cam,&buffers,currentBuffer]() {RenderJob(tracer,pos,chunk, winSize, cam, buffers[currentBuffer]); },&counters[currentBuffer]);
            }
        }
       
        JobSystem::Wait(&counters[currentBuffer]);
        window.DrawPixelBuffer(window.GetSize(), GL_RGB, GL_FLOAT, (void*)buffers[currentBuffer]);
        window.Update();
        currentBuffer++;
		if(currentBuffer >= FRAMES)
		{
            currentBuffer = 0;
		}
        printf("%f \n", timer.GetElapsedTimeInMS());
    }
 
    exit(EXIT_SUCCESS);
}