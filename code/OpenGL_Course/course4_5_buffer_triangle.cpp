#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "course4_5_buffer_triangle.h"
#include <Windows.h>


void course4_buffer(void)
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
     

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        Sleep(100);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_LINES, 0, 2);

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

    glfwTerminate();
    // close the glfw context of this thread
}