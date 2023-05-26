#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "course11_vertex_array.h"
#include <fstream>
#include <sstream>
#include <string>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
    // clear all the errors  
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) { // as long as error is not zero
        std::cout << "[OpenGL Error] error code: (" << error << "). In function: " <<
            function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderPromgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderPromgramSource ParseShader(const std::string& filepath) {
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

static unsigned int CompileShader(unsigned int type, const std::string& source) {
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

static unsigned int CreateShader(const std::string vertexShader,
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

void course11_vertex(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return;

    // set the major version of OpenGL is 3, minor version is also 3, 
    // so the version is OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // set OpenGL profile to be compatible version
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    // if we change it into the core profile, there will be error in 
    // glEnableVertexAttribArray(0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // you will need to terminate glfw before program end, or your porgram may stuck
    // if init failed, then return GLFW_FALSE, can call glfwTermiate altomatically
    // for thread safety, you will always need to call this function in main thread

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // set swap interval, every time the screen update, swap the memory
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {  // each line is a single points
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f,
    };
    unsigned int indices[] = { // each line is the index of the triangle
        0, 1, 2,
        2, 3, 0
    };
    // in course 11, create vertex array buffer, and bind it at the beginning
    // then the vertex buffer and the layout glVertexAttribPointer will be bind into vao
    unsigned int vao; // vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // course 4 content
    // we first generate a buffer in GPU, and return the pointer to that buffer
    unsigned int buffer;
    // 1 means we only want to define one buffer
    glGenBuffers(1, &buffer);
    // bind the buffer, tell GPU that this buffer is an array type
    // give the type of the buffer, and the id of buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // we put the data into the buffer
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    // now we only give openGL a bunch of bytes, we haven't tell openGL how to 
    // specify this data
    // we can enable this wherever we want since we bind to this buffer
    glEnableVertexAttribArray(0);
    // tell openGL how to specify this data

    // this function is actually link vao to buffer, and 0 is the vao location
    glVertexAttribPointer(0, //index is 0 because it is our first of the position
        2, // we have 2 components for each point
        GL_FLOAT, // float type
        GL_FALSE, // we don't want to normalize it
        2 * sizeof(float), // because 2 components * sizeof(float)
        (const void*)0 // the offset
    );

    // we first generate a buffer in GPU, and return the pointer to that buffer
    unsigned int ibo; // index buffer object
    glGenBuffers(1, &ibo);
    // give the type of the buffer, and the id of buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // we put the data into the buffer, use indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);

    // we take the file and divide it into 2 shaders file
    ShaderPromgramSource source = ParseShader("course10_uniform.shader");
    // return a program
    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    // finally we bind the shader
    glUseProgram(shader);

    // to start the uniform part, we need to first check out the location of the variable
    int location = glGetUniformLocation(shader, // our program
        "u_Color" // the name in our shader program
    );
    ASSERT(location != -1); // if location is -1, it is not a uniform variable

    // after use program, we can use uniform
    glUniform4f(location, // the location of our variable in shader
        0.0f, 0.3f, 0.8f, 1.0f);
    // please check the document for different glUniform function
    // here we use 4 float, because we use vec4

    // course 11 after we finin use this, we unbind the buffer
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    float r = 0.0f;
    float increment = 0.01f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // use program again
        glUseProgram(shader);

        glUniform4f(location, r, 0.3f, 0.8f, 1.0f);

        // bind buffer again
        // glBindBuffer(GL_ARRAY_BUFFER, buffer);
        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);
        // only need to bind vertex array buffer is okay
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        glDrawElements(GL_TRIANGLES, 6, // number of indices we are drawing
            GL_INT, nullptr // index to the indices buffer.
            // beacuse we use glBindBuffer to bound current object, we don't need this pointer
        );
        if (r > 1.0f) increment = -0.01f;
        else if (r < 0.0f) increment = 0.01f;
        r += increment;

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        // openGL use double buffer to prevent blink. all the drawing process
        // will not show on the screen until you call this function to map the 
        // drawing memory into the display memory

        /* Poll for and process events */
        glfwPollEvents();
        // only used in main thread, handle the event in the queue, which include
        // window moving, window size changing.
    }
    // finally we delete the shader
    glDeleteProgram(shader);

    glfwTerminate();
    // close the glfw context of this thread
}


