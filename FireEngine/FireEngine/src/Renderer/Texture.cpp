#include "Texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::Texture() : m_id(0), m_width(0), m_height(0), m_channels(0), m_isCubemap(false)
{
    glGenTextures(1, &m_id);
}

Texture::~Texture()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
    }
}

bool Texture::LoadFromFile(const std::string& path, bool flipVertical)
{
    stbi_set_flip_vertically_on_load(flipVertical ? 1 : 0);

    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (data == nullptr)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (m_channels == 1)
    {
        format = GL_RED;
    }
    else if (m_channels == 4)
    {
        format = GL_RGBA;
    }

    m_isCubemap = false;
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

bool Texture::LoadCubemap(const std::vector<std::string>& faces)
{
    if (faces.size() != 6)
    {
        return false;
    }

    m_isCubemap = true;
    stbi_set_flip_vertically_on_load(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (data == nullptr)
        {
            std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
            return false;
        }

        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void Texture::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::BindCubemap(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
