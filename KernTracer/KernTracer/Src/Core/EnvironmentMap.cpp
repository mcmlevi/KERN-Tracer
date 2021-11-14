#include "pch.h"
#include <Core/EnvironmentMap.h>
#include <glm/gtx/extended_min_max.hpp>
#include <Graphics/Texture.h>
#define M_PI   3.14159265358979323846264338327950288f
RT::EnvironmentMap::EnvironmentMap(const std::string& file)
{
	m_Texture = std::make_unique<RT::Texture>();
	m_Texture->LoadTexture(file,true);
}

glm::vec3 RT::EnvironmentMap::Intersect(const RT::Ray& ray)
{
	float phi = atan2(ray.direction.z, ray.direction.x);
	float theta = asin(ray.direction.y);
	float u = 1 - (phi + M_PI) / (2 * M_PI);
	float v = (theta + M_PI / 2) / M_PI;

	//float u{ (glm::atan(ray.direction.x / ray.direction.z) + M_PI) / (2.f * M_PI) };
	//float v{ (glm::asin(ray.direction.y) + 0.5f * M_PI) / M_PI };
	glm::ivec2 size = m_Texture->GetSize();
	int channels = m_Texture->GetChannels();

	u = glm::clamp (u, 0.f, 1.f);
	v = glm::clamp(v, 0.f, 1.f);
	int x{ static_cast<int>(u * size.x) };
	x = glm::clamp(x, 0, size.x);
	int y{ static_cast<int>(v * size.y - 1) };
	y = glm::clamp(y, 0, size.y);
	const unsigned char* colorPtr{ &m_Texture->GetTexture()[y * size.x * channels + x * channels] };
	return{ (*(colorPtr)) / 255.f, (*(colorPtr + 1)) / 255.f,(*(colorPtr + 2)) / 255.f};
}

