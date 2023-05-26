#include "GLCLass/Render.h"
#include "GLCLass/VertexBuffer.h"
#include "GLCLass/IndexBuffer.h"
#include "GLCLass/VertexArray.h"
#include "GLCLass/VertexBufferLayout.h"
#include "GLCLass/Shader.h"
#include "GLClass/Texture.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "course24_test_framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "test/TestClearColor.h"
#include "test/TestTexture2D.h"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void class24_test_framework(void)
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
    glEnable(GL_BLEND); // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how gl blend alpha channel
    Renderer renderer;

    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;
    testMenu->RegisterTest<test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<test::TestTexture2D>("2D Texture");

    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.Clear();

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (currentTest) {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-")) {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    delete currentTest;
    if(currentTest != testMenu)
        delete testMenu;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    // close the glfw context of this thread
}


