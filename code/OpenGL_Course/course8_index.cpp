#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "course8_index.h"
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>


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

void course8_index(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return;



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

    // course 4 content
    // we first generate a buffer in GPU, and return the pointer to that buffer
    unsigned int buffer;
    // 1 means we only want to define one buffer
    glGenBuffers(1, &buffer);
    // bind the buffer, tell GPU that this buffer is an array type
    // give the type of the buffer, and the id of buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // we put the data into the buffer
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    // now we only give openGL a bunch of bytes, we haven't tell openGL how to 
    // specify this data
    
    // we first generate a buffer in GPU, and return the pointer to that buffer
    unsigned int ibo; // index buffer object
    glGenBuffers(1, &ibo);
    // give the type of the buffer, and the id of buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // we put the data into the buffer, use indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);
    

    // we can enable this wherever we want since we bind to this buffer
    glEnableVertexAttribArray(0);
    // tell openGL how to specify this data
    glVertexAttribPointer(0, //index is 0 because it is our first of the position
        2, // we have 2 components for each point
        GL_FLOAT, // float type
        GL_FALSE, // we don't want to normalize it
        2 * sizeof(float), // because 2 components * sizeof(float)
        (const void*)0 // the offset
    );

    // we take the file and divide it into 2 shaders file
    ShaderPromgramSource source = ParseShader("course7_basic.shader");
    // return a program
    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    // finally we bind the shader
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        Sleep(100);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
        // glDrawArrays(GL_LINES, 0, 2);
        glDrawElements(GL_TRIANGLES, 6, // number of indices we are drawing
            GL_UNSIGNED_INT, nullptr // index to the indices buffer.
            // beacuse we use glBindBuffer to bound current object, we don't need this pointer
        );

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


