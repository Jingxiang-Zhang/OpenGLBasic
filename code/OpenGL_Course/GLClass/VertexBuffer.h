#pragma once

class VertexBuffer 
{
private:
	// the pointer to OpenGL vertex
	unsigned int m_RenderID;
	const int m_DynamicAllocationSize = 1024;
public:
	VertexBuffer(); // allocate dynamically
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void BufferSubData(unsigned int offset, unsigned int size, float* data) const;

	// bind the vertex that we want to draw next
	void Bind() const;
	// unbind the vertex that we don't want to draw next
	void UnBind() const;
};
