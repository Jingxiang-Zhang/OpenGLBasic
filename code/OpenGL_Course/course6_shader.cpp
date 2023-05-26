#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "course6_shader.h"
#include <Windows.h>

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
        std::cout << "Failed to compile "<< 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            <<"shader!" << std::endl;
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

void course6_shader(void)
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

    float positions[6] = {  // each line is a vertex
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
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

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    // now we only give openGL a bunch of bytes, we haven't tell openGL how to 
    // specify this data

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

    std::string vertexShader =
        "#version 330 core\n"  // glsl language. 
        // core means not allowed to use deprecated function
        // for version 330, we don't need new feature of 450 for now, 
        // just want to make sure compatible
        "\n"
        "layout(location = 0) in vec4 position;\n"  // 0 mean the first parameter
        // in glVertexAttribPointer function. it's the index of our attribute
        // we use vec4 to receive the data because gl_Position is vec4 type
        // vertex shader help us draw the position of vertex, we will input the position
        // and we only need to set the gl_Position variable to tell OpenGL the position
        "void main()\n"  // this main function will be called when the vertex get 
        // called. each vertex will call this function independently
        "{\n"
        "   gl_Position = position;\n"  // set gl position equal to our position
        // if we use vec2 to receive data, we will convert it in vec4 to match gl_Position
        // gl_Position = vec4(position.xy, 0.0, 1.0);
        // instead, if we use vec4 to define the receive parameter position, 
        // OpenGL know it is a vec2 type because we define this in the second parameter
        // of glVertexAttribPointer function, and will help us convert to vec4
        "}\n";

        
    std::string fragmentShader =
        "#version 330 core\n" 
        "\n"
        "layout(location = 0) out vec4 color;\n" // remember, fragment shader help
        // us color the graphic, it does not take any input for this case
        // but it will need to output color
        // 
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 0.0);\n"  // red color, since it use rgba
        // 1.0 means 255, and 0 means 0
        "}\n";

    // return a program
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    // finally we bind the shader
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        Sleep(100);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawArrays(GL_LINES, 0, 2);

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