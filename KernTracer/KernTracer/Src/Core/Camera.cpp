#include "pch.h"
#include "Core/Camera.h"
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>


RT::Camera::Camera(const float fov, const glm::vec2& size, const glm::vec3& origin, const glm::vec3& lookat):m_fov{fov},m_size{size},m_position{origin},m_lookAt{lookat}
{
	m_view = glm::lookAt(m_position, m_lookAt, { 0,-1,0 });
	m_aspectRatio = m_size.x / m_size.y;
	m_angle = (glm::tan((glm::pi<float>() * m_fov / 180.f) / 2.f));
	m_proj = glm::perspective(-glm::radians(fov), m_aspectRatio, 0.1f, 1000.f);
	
}

void RT::Camera::UpdateCamera(const glm::vec2& size)
{
	m_size = size;
	m_aspectRatio = m_size.x / m_size.y;
	m_proj = glm::perspective(-glm::radians(m_fov), m_aspectRatio, 0.1f, 1000.f);
}

RT::Ray RT::Camera::GetRayAt(const glm::vec2& screenPos) const
{
	// normalize screen coordinates and make sure the values are from the middle of the pixel

	float normalX{ (screenPos.x + 0.5f) / m_size.x };
	float normalY{ (screenPos.y + 0.5f) / m_size.y };
	
	// Calculate the
	float wX{ (2.f * normalX - 1.f) * m_aspectRatio * m_angle };
	float wY{ (1.f - 2.f * normalY) * m_angle };
	const glm::vec3 raydir{ glm::normalize(m_view * glm::vec4{wX, wY, -1.f,0} ) };
	//const glm::vec3 origin{ m_view * glm::vec4{m_position,1.f} };

	return Ray{ m_position,raydir };
}

const glm::mat4& RT::Camera::GetProjection() const
{
	return m_proj;
}

glm::mat4 RT::Camera::GetView() const
{
	return m_view;
}
