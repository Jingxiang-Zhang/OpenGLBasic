#include <GLFW/glfw3.h>
#include "course2_glfw.h"

void draw_triangle() {
    // we use legacy way
    glBegin(GL_TRIANGLES);
    glVertex2d(-0.5f, -0.5f);
    glVertex2d(0.0f, 0.5f);
    glVertex2d(0.5f, -0.5f);
    glEnd();
}

void course2_glfw(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return ;
    // you will need to terminate glfw before program end, or your porgram may stuck
    // if init failed, then return GLFW_FALSE, can call glfwTermiate altomatically
    // for thread safety, you will always need to call this function in main thread

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return ;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        draw_triangle();

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