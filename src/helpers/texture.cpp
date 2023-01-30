#include "texture.h"
#include "helpers/fs.h"
#include "helpers/logger.h"
#include "renderer/renderer.h"
#include <stb_image.h>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

Texture::Texture(const fs::path &path)
{
    int image_width = 0;
    int image_height = 0;
    uint8_t *data = stbi_load(path.c_str(), &image_width, &image_height, NULL, 4);

    if (data)
    {
        this->m_TextureID = Renderer::ConstructTexture(data, image_width, image_height);
        this->m_Width = image_width;
        this->m_Height = image_height;
        this->m_IsLoaded = true;
        stbi_image_free(data);
    }
    else 
    {
        TS_LOG_ERROR("Could not load texture from: '%s'", path.c_str());
    }
}

Texture::Texture(const uint8_t *data, int image_width, int image_height) : m_Width(image_width), m_Height(image_height)
{
    this->m_TextureID = Renderer::ConstructTexture(data, image_width, image_height);
    this->m_IsLoaded = true;
}

Texture::~Texture()
{
    Renderer::DestroyTexture(m_TextureID);
}
