#include <pch.h>
#include <Graphics/Texture.h>
#define STB_IMAGE_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <stbi/stb_image.h>
#pragma GCC diagnostic pop
RT::Texture::~Texture()
{
	stbi_image_free(m_pixelArray);
}

void RT::Texture::LoadTexture(const std::string& filePath)
{
	//stbi_set_flip_vertically_on_load(true);
	m_pixelArray = stbi_load(filePath.c_str(), &m_size.x, &m_size.y, &m_channels,STBI_rgb);
}

const unsigned char* RT::Texture::GetTexture() const
{
	return m_pixelArray;
}

glm::ivec2 RT::Texture::GetSize() const
{
	return m_size;
}

int32_t RT::Texture::GetChannels() const
{
	return m_channels;
}
