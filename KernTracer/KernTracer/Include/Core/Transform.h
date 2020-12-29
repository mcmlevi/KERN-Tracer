#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
namespace RT
{
	class Transform
	{
	public:
		Transform(const glm::vec3& rot = {0.f,0.f,0.f}, const glm::vec3& pos = { 0.f,0.f,0.f }, const glm::vec3& scale = { 1.f,1.f,1.f });
		void SetPosition(const glm::vec3& newPos);
		void Translate(const glm::vec3& translate);
		const glm::vec3& GetPosition() const;
		// Radians
		void SetRotation(const glm::vec3& newRot);
		// Radians
		void Rotate(const glm::vec3& rotation); 
		const glm::vec3& GetRotation() const;

		void SetScale(const glm::vec3& newScale);
		// Multiplies
		void Scale(const glm::vec3& scaleFactor);
		const glm::vec3& GetScale() const;

		const glm::mat4& GetTransform() const;
		const glm::mat4& GetInverse() const;

	private:
		void Recalculate();
		
		glm::vec3 m_rotation{};
		glm::vec3 m_position{};
		glm::vec3 m_scale{1.f,1.f,1.f};
		glm::mat4 m_transform{};
		glm::mat4 m_inverse{};
		
	};
}


