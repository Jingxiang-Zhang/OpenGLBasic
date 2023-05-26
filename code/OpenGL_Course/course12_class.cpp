#include "GLCLass/Render.h"
#include "GLCLass/VertexBuffer.h"
#include "GLCLass/IndexBuffer.h"
#include "GLCLass/VertexArray.h"
#include "GLCLass/VertexBufferLayout.h"
#include "GLCLass/Shader.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "course12_class.h"


void class12_use_class(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return;
    // set the major version of OpenGL is 3, minor version is also 3, 
    // so the version is OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);
    // set swap interval, every time the screen update, swap the memory
    glfwSwapInterval(1); 
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    { // we bracket here because we define vb and ib in stack memory space
        // glfwTerminate will delete all the context before the destructor called in vb and ib
        // then when main function end, vb and ib can not be deleted
        // we can bracket vb and ib, when the bracket end, it will be released
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

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("shader/course10_uniform.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    Renderer renderer;

    float r = 0.0f;
    float increment = 0.01f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        renderer.Draw(va, ib, shader);

        if (r > 1.0f) increment = -0.01f;
        else if (r < 0.0f) increment = 0.01f;
        r += increment;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // finally we delete the shader
    }

    glfwTerminate();
    // close the glfw context of this thread
}


