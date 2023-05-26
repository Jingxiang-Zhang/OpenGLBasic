#include "GLCLass/Render.h"
#include "GLCLass/VertexBuffer.h"
#include "GLCLass/IndexBuffer.h"
#include "GLCLass/VertexArray.h"
#include "GLCLass/VertexBufferLayout.h"
#include "GLCLass/Shader.h"
#include "GLClass/Texture.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "course22_imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void class22_imgui(void)
{
    glfwSetErrorCallback(glfw_error_callback);
    // Initialize the library
    if (!glfwInit())
        return;
    // set the major version of OpenGL is 3, minor version is also 3, 
    // so the version is OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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

    // start imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 100");

    std::cout << glGetString(GL_VERSION) << std::endl;

    { // we bracket here because we define vb and ib in stack memory space
        // glfwTerminate will delete all the context before the destructor called in vb and ib
        // then when main function end, vb and ib can not be deleted
        // we can bracket vb and ib, when the bracket end, it will be released
        float positions[] = {  // each line is a single points + the coordinate of the texture
            -50.0f, -50.0f, 0.0f, 0.0f,
            50.0f, -50.0f, 1.0f, 0.0f,
            50.0f, 50.0f, 1.0f, 1.0f,
            -50.0f, 50.0f, 0.0f, 1.0f,
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

        glm::mat4 project = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::vec3 translationA(200.0f, 200.0f, 0);
        glm::vec3 translationB(400.0f, 200.0f, 0);

        Shader shader("shader/course19_projection.shader");
        shader.Bind();

        Renderer renderer;

        Texture texture("img/test.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // imgui variable
        
        //bool show_demo_window = true;
        //bool show_another_window = false;
        //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SliderFloat3("Translation A", &translationA.x, 0, 960.0f);
            ImGui::SliderFloat3("Translation B", &translationB.x, 0, 960.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            //if (show_demo_window)
            //    ImGui::ShowDemoWindow(&show_demo_window);
            //{
            //    static float f = 0.0f;
            //    static int counter = 0;
            //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            //    ImGui::Checkbox("Another Window", &show_another_window);

            //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            //        counter++;
            //    ImGui::SameLine();
            //    ImGui::Text("counter = %d", counter);

            //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            //    ImGui::End();
            //}
            //if (show_another_window)
            //{
            //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            //    ImGui::Text("Hello from another window!");
            //    if (ImGui::Button("Close Me"))
            //        show_another_window = false;
            //    ImGui::End();
            //}


            // Render here 
            renderer.Clear();
            {
                // render the first image
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = project * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_modelViewProjection", mvp);
                renderer.Draw(va, ib, shader);
            }
            {
                // render the second image
                // render it one by one is slow, instead, we can shovel all the 
                // objects into one vertex buffer, and call the draw function 
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = project * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_modelViewProjection", mvp);
                renderer.Draw(va, ib, shader);
            }
            

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    // close the glfw context of this thread
}


