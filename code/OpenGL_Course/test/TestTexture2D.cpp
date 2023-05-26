#include "TestTexture2D.h"
#include "GL/glew.h"
#include "../GLClass/Render.h"
#include "../vendor/imgui/imgui.h"




namespace test {

	TestTexture2D::TestTexture2D()
        :m_TranslationA(200.0f, 200.0f, 0), m_TranslationB(400.0f, 200.0f, 0),
        m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), 
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
        glEnable(GL_BLEND); // enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set how gl blend alpha channel
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

        m_VAO = std::make_unique<VertexArray>();

        // create vertex buffer
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * (2 + 2) * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2); // append the point position
        layout.Push<float>(2); // append the coordinate of the texture
        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        // create index buffer
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        // create shader
        m_Shader = std::make_unique<Shader>("shader/course19_projection.shader");
        m_Shader->Bind();

        // loader texture
        m_Texture = std::make_unique<Texture>("img/test.png");
        m_Shader->SetUniform1i("u_Texture", 0);
	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{

	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
        Renderer renderer;
        m_Texture->Bind();
        
        {
            // render the first image
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_modelViewProjection", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
        {
            // render the second image
            // render it one by one is slow, instead, we can shovel all the 
            // objects into one vertex buffer, and call the draw function 
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_modelViewProjection", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0, 960.0f);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0, 960.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / 
            ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	}
}
