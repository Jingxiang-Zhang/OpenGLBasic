#pragma once
#include "Render.h"
#include "Shader.h"

class Texture {
private:
	unsigned int m_RenderID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP; // BPP: bits per pixel

public:
	Texture();  // white texture
	Texture(const std::string& path); // load one texture
	~Texture();

	// The slot here is the channel of the texture. You can bind more than one texture
	// at a time, and in modern GPU, you can bind 32 textures at a time. 
	// While in mobile phone, you can have 8 textures or more at a time.
	// it depends on the GPU
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

	inline unsigned int getId() const { return m_RenderID; }
};

class Textures {
public:
	struct ImgInfo {
		int Width;
		int Height;
		int BPP;
	};
private:
	std::vector<unsigned int> m_RenderIDArray;
	std::vector<std::string> m_FilePathArray;
	unsigned char* m_LocalBuffer;
	std::vector<ImgInfo> m_ImgInfo;
public:
	Textures(const std::vector<std::string>& path); // load multiple textures
	~Textures();

	void Bind() const;
	void Unbind() const;
};
