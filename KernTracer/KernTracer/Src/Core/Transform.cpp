#include "pch.h"
#include <Core/Transform.h>
#include <glm/gtc/matrix_transform.hpp>

RT::Transform::Transform(const glm::vec3& rot, const glm::vec3& pos, const glm::vec3& scale):m_rotation{rot},m_position{pos},m_scale{scale}
{
	Recalculate();
}

void RT::Transform::SetPosition(const glm::vec3& newPos)
{
	m_position = newPos;
	Recalculate();
}

void RT::Transform::Translate(const glm::vec3& translate)
{
	m_position += translate;
	Recalculate();
}

const glm::vec3& RT::Transform::GetPosition()const
{
	return  m_position;
}

void RT::Transform::SetRotation(const glm::vec3& newRot)
{
	m_rotation = newRot;
	Recalculate();
}

void RT::Transform::Rotate(const glm::vec3& rotation)
{
	m_rotation += rotation;
	Recalculate();
}

const glm::vec3& RT::Transform::GetRotation()const
{
	return m_rotation;
}

void RT::Transform::SetScale(const glm::vec3& newScale)
{
	m_scale = newScale;
	Recalculate();
}

void RT::Transform::Scale(const glm::vec3& scaleFactor)
{
	m_scale *= scaleFactor;
}

const glm::vec3& RT::Transform::GetScale()const
{
	return m_scale;
}

const glm::mat4& RT::Transform::GetTransform()const
{
	return m_transform;
}

const glm::mat4& RT::Transform::GetInverse()const
{
	return m_inverse;
}

void RT::Transform::Recalculate()
{
	glm::mat4 translation{glm::translate(glm::identity<glm::mat4>(),m_position)};
	glm::mat4 rotation{ glm::identity<glm::mat4>() };
	rotation = glm::rotate(rotation, m_rotation.x, glm::vec3{ 1.f,0.f,0.f });
	rotation = glm::rotate(rotation, m_rotation.y, glm::vec3{ 0.f,1.f,0.f });
	rotation = glm::rotate(rotation, m_rotation.z, glm::vec3{ 0.f,0.f,1.f });
	glm::mat4 scale{glm::scale(glm::identity<glm::mat4>(),m_scale)};
	m_transform = translation * rotation * scale;
	m_inverse = glm::inverse(m_transform);
}
