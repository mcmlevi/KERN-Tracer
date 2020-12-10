#include "pch.h"
#include <Core/RayTracer.h>
#include <Core/Triangle.h>
#include <Core/ObjLoader.h>
RT::ObjLoader loader;
std::unique_ptr<RT::Model> model = loader.LoadModel("cow.obj");

glm::vec3 RT::RayTracer::Trace(const Ray& ray, int currentDepth)
{
	float t;
	bool hit{false};
	for (int i = 0; i < model->triangles.size(); ++i)
	{
		if (model->triangles[i].intersect(ray, t))
		{
			hit = true;
			break;
		}
	}

	if (hit)
		return { 1,0,0 };
	else
		return { 0,1,0 };
}
