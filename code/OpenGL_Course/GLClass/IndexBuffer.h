 #pragma once

class IndexBuffer
{
private:
	// the pointer to OpenGL vertex
	unsigned int m_RenderID;
	// how many indices it has
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	// bind the vertex that we want to draw next
	void Bind() const;
	// unbind the vertex that we don't want to draw next
	void UnBind() const;

	inline unsigned int GetCount() const { return m_Count; }
};
