#pragma once
#include <glm/vec3.hpp>
#include <memory>
#include <Graphics/Texture.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <Core/Triangle.h>
namespace RT
{
	class Triangle;
	struct Material
	{
		glm::vec3 baseColor{ 1.f,1.f,1.f };
		float reflectiveIndex = 0;
		float refractiveIndex = 0;
		std::shared_ptr<Texture> texture;
		glm::vec3 GetTexture(const glm::vec3 intersectionPoint,
			const RT::Triangle& triangle) const;
	};

	inline glm::vec3 Material::GetTexture(const glm::vec3 intersectionPoint,
	                                      const RT::Triangle& triangle) const
	{
		if (texture != nullptr)
		{
			glm::vec3 v0 = triangle.GetVerts()[1] - triangle.GetVerts()[0], v1 = triangle.GetVerts()[2] - triangle.GetVerts()[0], v2 = intersectionPoint - triangle.GetVerts()[0];
			float d00 = glm::dot(v0, v0);
			float d01 = glm::dot(v0, v1);
			float d11 = glm::dot(v1, v1);
			float d20 = glm::dot(v2, v0);
			float d21 = glm::dot(v2, v1);
			float denom = d00 * d11 - d01 * d01;
			float v = glm::clamp((d11 * d20 - d01 * d21) / denom,0.f,1.f);
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = glm::clamp(1.0f - v - w, 0.f, 1.f);

			glm::vec2 P = triangle.GetTextureCoords()[0] * v + triangle.GetTextureCoords()[1] * w  + triangle.GetTextureCoords()[2] * u;
			
			const unsigned char* texptr = texture->GetTexture();
			glm::ivec2 textureSize = texture->GetSize();
			int32_t channels = texture->GetChannels();

			int x{ static_cast<int>(P.x * textureSize.x) };
			x = glm::clamp(x, 0, textureSize.x);
			int y{ static_cast<int>(P.y * textureSize.y - 1) };
			y = glm::clamp(y, 0, textureSize.y);
			int index = { (y * textureSize.x * channels + x * channels)};
			//return glm::vec3{ 1.f,1.f,1.f };
			return glm::vec3{ static_cast<float>(texptr[index] / 255.f),static_cast<float>(texptr[index + 1] / 255.f),static_cast<float>(texptr[index + 2] / 255.f) };
	


			//float row = u * (float)textureSize.x;
			//float collum = v * (float)textureSize.y;
			//uint32_t index{ static_cast<uint32_t>(row * channels + collum * channels * textureSize.y )};
			//return glm::vec3{ static_cast<float>(texptr[index]) / 255.f,static_cast<float>(texptr[index + 1] / 255.f),static_cast<float>(texptr[index + 2] / 255.f) };
		}
		return { 1.f,1.f,1.f };
	}
}
