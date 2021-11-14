#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <Core/Ray.h>

namespace RT
{
	class Camera
	{
	public:
		Camera(const float fov, const glm::vec2& size, const glm::vec3& origin, const glm::vec3& lookat);
		void UpdateCamera(const glm::vec2& size);
		Ray GetRayAt(const glm::vec2& screenPos) const;
		const glm::mat4& GetProjection() const;
		glm::mat4 GetView() const;
	private:
		float m_fov;
		float m_aspectRatio;
		float m_angle;
		
		glm::vec2 m_size;
		glm::vec3 m_position;
		glm::vec3 m_lookAt;
		glm::mat4 m_view;
		glm::mat4 m_proj;

	};

}

