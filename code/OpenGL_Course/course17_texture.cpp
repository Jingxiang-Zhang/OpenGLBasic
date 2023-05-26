#include "GLCLass/Render.h"
#include "GLCLass/VertexBuffer.h"
#include "GLCLass/IndexBuffer.h"
#include "GLCLass/VertexArray.h"
#include "GLCLass/VertexBufferLayout.h"
#include "GLCLass/Shader.h"
#include "GLClass/Texture.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "course17_texture.h"


void class17_texture(void)
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
        float positions[] = {  // each line is a single points + the coordinate of the texture
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f,
        };
        unsigned int indices[] = { // each line is the index of the triangle
            0, 1, 2,
            2, 3, 0
        };

        glEnable(GL_BLEND); // enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how gl blend alpha channel

        VertexArray va;
        VertexBuffer vb(positions, 4 * (2 + 2) * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2); // append the point position
        layout.Push<float>(2); // append the coordinate of the texture
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);
        Shader shader("shader/course17_texture.shader");
        shader.Bind();
        Renderer renderer;

        Texture texture("img/test.png");
        // create a white texture
        // Texture texture;
        const int slot = 0;
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        while (!glfwWindowShouldClose(window)) {
            /* Render here */
            renderer.Clear();
            shader.Bind();
            renderer.Draw(va, ib, shader);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    // close the glfw context of this thread
}


