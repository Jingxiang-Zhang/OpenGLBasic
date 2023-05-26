#pragma once
#include <iostream>
#include <GL/glew.h>
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

// Assert false will interrupt the program
#define ASSERT(x) if (!(x)) __debugbreak();
// by using GLCall, when OpenGL error happened, the program will be interrupted
#ifdef _DEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x;
#endif

// flush all the error in OpenGL error stack
static void GLClearError() {
    // clear all the errors  
    while (glGetError() != GL_NO_ERROR);
}

// print the current OpenGL error
static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) { // as long as error is not zero
        std::cout << "[OpenGL Error] error code: (" << error << "). In function: " <<
            function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


class Renderer 
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};