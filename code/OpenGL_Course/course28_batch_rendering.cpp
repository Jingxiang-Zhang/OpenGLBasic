#include "GLCLass/Render.h"
#include "GLCLass/VertexBuffer.h"
#include "GLCLass/IndexBuffer.h"
#include "GLCLass/VertexArray.h"
#include "GLCLass/VertexBufferLayout.h"
#include "GLCLass/Shader.h"
#include "GLClass/Texture.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "course28_batch_rendering.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include <array>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

struct vec2 {
    float x, y;
};

struct Vertex {
    vec2 Position;
    vec2 TexCoords;
    float TexID;
};

static std::array<Vertex, 4> CreateSquad(float x, float y, float length, float texId) {
    Vertex v0;
    v0.Position = { x, y };
    v0.TexCoords = { 0.0f, 0.0f };
    v0.TexID = texId;
    
    Vertex v1;
    v1.Position = { x + length, y };
    v1.TexCoords = { 1.0f, 0.0f };
    v1.TexID = texId;

    Vertex v2;
    v2.Position = { x + length, y + length };
    v2.TexCoords = { 1.0f, 1.0f };
    v2.TexID = texId;

    Vertex v3;
    v3.Position = { x, y + length };
    v3.TexCoords = { 0.0f, 1.0f };
    v3.TexID = texId;

    return { v0, v1, v2, v3 };
}

std::vector<unsigned int> CreateSquadIndex(unsigned int nSquad) {
    std::vector<unsigned int> indices;
    indices.reserve(nSquad * 6 + 1);
    for (int i = 0; i < nSquad; i++) {
        int begin = i * 4;
        indices.push_back(begin);
        indices.push_back(begin + 1);
        indices.push_back(begin + 2);
        indices.push_back(begin + 2);
        indices.push_back(begin + 3);
        indices.push_back(begin);
    }
    return indices;
}

void class28_batch_rendering(void)
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

    Vertex vertex[10];
    { 
        //float positions[] = {
        //    -50.0f, -50.0f, 0.0f, 0.0f, 0.0f,
        //    50.0f, -50.0f, 1.0f, 0.0f, 0.0f,
        //    50.0f, 50.0f, 1.0f, 1.0f, 0.0f,
        //    -50.0f, 50.0f, 0.0f, 1.0f, 0.0f,
        //    200.0f, -50.0f, 0.0f, 0.0f, 1.0f,
        //    300.0f, -50.0f, 1.0f, 0.0f, 1.0f,
        //    300.0f, 50.0f, 1.0f, 1.0f, 1.0f,
        //    200.0f, 50.0f, 0.0f, 1.0f, 1.0f
        //};
        //unsigned int indices[] = { // each line is the index of the triangle
        //    0, 1, 2,
        //    2, 3, 0,
        //    4, 5, 6,
        //    6, 7, 4
        //};

        std::array<Vertex, 4> squad1 = CreateSquad(-50.0f, -50.0f, 100.0f, 0);
        std::array<Vertex, 4> squad2 = CreateSquad(200.0f, -50.0f, 100.0f, 1);
        Vertex* vertex = new Vertex[8];
        memcpy(vertex, squad1.data(), sizeof(Vertex) * 4);
        memcpy(vertex + 4, squad2.data(), sizeof(Vertex) * 4);

        std::vector<unsigned int> indices = CreateSquadIndex(2);

        glEnable(GL_BLEND); // enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how gl blend alpha channel

        VertexArray va;
        // VertexBuffer vb(positions, 8 * (2 + 2 + 1) * sizeof(float));
        VertexBuffer vb;
        VertexBufferLayout layout;
        layout.Push<float>(2); // append the point position
        layout.Push<float>(2); // append the coordinate of the texture
        layout.Push<float>(1); // append the texture id of the texture
        va.AddBuffer(vb, layout);
        vb.BufferSubData(0, 8 * (2 + 2 + 1) * sizeof(float), (float*)vertex);

        IndexBuffer ib(indices.data(), 2 * 6);

        glm::mat4 project = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::vec3 translation(200.0f, 200.0f, 0);

        Shader shader("shader/course28_batch_rendering.shader");
        shader.Bind();

        Renderer renderer;

        std::vector<std::string> path_list = {
            "img/test.png", "img/test.jpg"
        };
        Textures textures(path_list);
        textures.Bind();
        int sampler[2] = { 0, 1 };
        shader.SetUniform1iv("u_Texture", 2, sampler);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SliderFloat3("Translation", &translation.x, 0, 960.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            // Render here 
            renderer.Clear();
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
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


