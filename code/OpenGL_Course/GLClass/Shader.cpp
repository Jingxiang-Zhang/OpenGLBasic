#include "Shader.h"
#include "Render.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>



Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RenderID(0)
{
    ShaderPromgramSource source = ParseShader(filepath);
    m_RenderID = CreateShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader() {
    GLCall(glDeleteProgram(m_RenderID));
}


ShaderPromgramSource Shader::ParseShader(const std::string filepath) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cout << "shader file open failed!" << std::endl;
    }
    // define shader type
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2]; // pile up the shader together
    ShaderType type = ShaderType::NONE; // record current type

    while (getline(stream, line)) {
        // if find the shader type define, we set the mode
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                // set mode to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                // set mode to fragment
                type = ShaderType::FRAGMENT;
        }
        // else, we add it into the source code
        else {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    // declare vertex shader or fragment shader
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, // shader id
        1, // how many source code we want to define
        &src, // memory address of the code, double pointer. we only have one
        // source code, so we take the address of this pointer
        nullptr // not important
    );
    glCompileShader(id); // specify which shader we want to compile

    // TODO: Error handling
    // the function above does not return anything about error, 
    // but we can query the error msg
    int result;
    glGetShaderiv(id, // shader id
        GL_COMPILE_STATUS, // consult compile status
        &result // return pointer
    ); // iv means: i is integer, v is vector
    // now we noly know there is error happened or not. we still need to know the msg
    if (result == GL_FALSE) {
        // get the length of the msg
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // you can define your message of the heap memory, but you will need to 
        // free the memory after using it.
        // char* message = new char[length];
        // you can also dynamically apply for memory in the statck memory area by
        // using alloca function, and it will be freeed automatically
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader!" << std::endl;
        std::cout << message << std::endl;
        // delete this shader
        glDeleteShader(id);
        return 0;
    }
    return id;
}

unsigned int Shader::CreateShader(const std::string vertexShader,
    const std::string fragmentShader) {
    // declare a program pointer
    unsigned int program = glCreateProgram();
    // code duplicate for write two shader, so I will use a new function
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    // combine this two code to the program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    // link the program and validate program
    glLinkProgram(program);
    glValidateProgram(program);

    // now we can delete our shader program, because we have already compile our 
    // shader program to GPU, we don't need the source code
    glDeleteShader(vs);
    glDeleteShader(fs);
    // you should call glDetachShader after it been linked, we'll talk it later
    return program;
} 

void Shader::Bind() const {
    GLCall(glUseProgram(m_RenderID));
}
void Shader::UnBind() const {
    GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1i(const std::string& name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1iv(const std::string& name, int length, int* value) {
    GLCall(glUniform1iv(GetUniformLocation(name), length, value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}


int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationChche.find(name) != m_UniformLocationChche.end()) {
        return m_UniformLocationChche[name];
    }
    GLCall(int location = glGetUniformLocation(m_RenderID, name.c_str()));
    if (location == -1) {
        std::cout << "warning: uniform " << name << " doesn't exist!" << std::endl;
    }
    m_UniformLocationChche[name] = location;
    return location;
}