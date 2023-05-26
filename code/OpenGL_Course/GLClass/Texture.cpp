#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path)
	: m_RenderID(0), m_FilePath(path), m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0)
{
	// in OpenGL, the image is from bottom to top, we need to flip it
	stbi_set_flip_vertically_on_load(1);
	// 4 means 4 channels: R G B A
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    // generate texture
    GLCall(glGenTextures(1, &m_RenderID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderID));

    // upload image to OpenGL
    // first 0 means not a multi-level texture, second 0 is that we don't want a boarder
    // the first group of data is the internal format that how the data store in OpenGL
    // the second group of data is the format of image that you want to upload
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

    // set the texture parameters
    // generate the mipmaps for this texture
    // For the GL_TEXTURE_MIN_FILTER parameter, you can have: 1. GL_LINEAR_MIPMAP_LINEAR, 2. GL_LINEAR
    // if you use GL_LINEAR_MIPMAP_LINEAR, you need to first glGenerateMipmap
    // and you also need to upload image using glTexImage2D first before generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    // set when the image is rander smaller, use mipmap linear interpolation.
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    // set when image is rander larger, use linear interpolation
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // set when the coordinate is outside [0,1] in S, then repeat the image 
    // for GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T, you can set 1. GL_REPEAT, 2. GL_CLAMP_TO_EDGE
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    // set when the coordinate is outside [0,1] in T, then repeat the image
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // query support for anisotropic texture filtering
    
    // unbind the texture
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    if (m_LocalBuffer) {
        stbi_image_free(m_LocalBuffer);
    }
}

Texture::Texture()
    : m_RenderID(0), m_FilePath(), m_LocalBuffer(nullptr),
    m_Width(1), m_Height(1), m_BPP(4) 
{
    GLCall(glGenTextures(1, &m_RenderID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderID));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    uint32_t color = 0xffffffff;
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, &color));
}

Texture::~Texture() {
    GLCall(glDeleteTextures(1, &m_RenderID));
}

void Texture::Bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderID));
}

void Texture::Unbind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Textures::Textures(const std::vector<std::string>& path)
    :m_RenderIDArray(), m_FilePathArray(path), 
    m_LocalBuffer(nullptr), m_ImgInfo()
{
    stbi_set_flip_vertically_on_load(1);
    // generate texture
    unsigned int* renderIDArray = new unsigned int[path.size()];
    GLCall(glGenTextures(path.size(), renderIDArray));
    for (size_t i = 0; i < path.size(); i++) {
        std::string path_ = m_FilePathArray[i];
        m_RenderIDArray.push_back(renderIDArray[i]);
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RenderIDArray[i]));
        ImgInfo imgInfo;
        m_LocalBuffer = stbi_load(path_.c_str(), &imgInfo.Width, 
            &imgInfo.Height, &imgInfo.BPP, 4);
        m_ImgInfo.push_back(imgInfo);

        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgInfo.Width, 
            imgInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
        glGenerateMipmap(GL_TEXTURE_2D);
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        if (m_LocalBuffer) {
            stbi_image_free(m_LocalBuffer);
        }
    }
    delete[] renderIDArray;
}

Textures::~Textures() {
    for (auto RenderId : m_RenderIDArray) {
        GLCall(glDeleteTextures(1, &RenderId));
    }
}

void Textures::Bind() const {
    for (int i = 0; i < m_RenderIDArray.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_RenderIDArray[i]);
    }
}

void Textures::Unbind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

