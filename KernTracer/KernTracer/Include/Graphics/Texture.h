#pragma once
#include <glm/vec2.hpp>
#include <string>
namespace RT
{
	class Texture
	{
	public:
		~Texture();
		void LoadTexture(const std::string& filePath);
		const unsigned char* GetTexture() const;
		glm::ivec2 GetSize() const;
		int32_t GetChannels() const;
	private:
		unsigned char* m_pixelArray;
		glm::ivec2 m_size;
		int32_t m_channels;
	};
}
