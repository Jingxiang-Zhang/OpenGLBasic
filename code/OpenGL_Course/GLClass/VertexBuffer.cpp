#include "VertexBuffer.h"
#include "Render.h"

VertexBuffer::VertexBuffer() {
    GLCall(glGenBuffers(1, &m_RenderID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, m_DynamicAllocationSize, 
        nullptr, GL_DYNAMIC_DRAW));
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RenderID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RenderID));
}

void VertexBuffer::BufferSubData(unsigned int offset, unsigned int size, float* data) const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}


void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
}

void VertexBuffer::UnBind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
