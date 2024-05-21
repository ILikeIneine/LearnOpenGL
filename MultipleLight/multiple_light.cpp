#include <format>
#include <iostream>
#include <filesystem>

// third_party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "camera.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PROJECT_NAME "MultipleLight"

// settings
constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 1600;

float mixValue = 0.2f;

// Non-changed
auto cameraPos   = glm::vec3(0.0f, 0.0f, 6.0f);
auto cameraUp    = glm::vec3(0.0, 1.0, 0.0);
auto cameraFront = glm::vec3(0.0, 0.0, -1.0);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = SCR_WIDTH  / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera{ {-0.2f, 0.3f, 5.0} };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const std::filesystem::path&);


int main()
{
    //-------------------------------------
    // glfw: initialize and configure
    //-------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //-------------------------------------
    // glfw window creation
    //-------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //----------------------------------------
    // glad: load all OpenGL function pointers
    //----------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    //-------------------------------------
    // Create shader
    //-------------------------------------
    Shader lightingShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/material.vs", 
                           std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/material.fs");
    Shader lightCubeShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.vs",
                      std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.fs");
    Shader axisShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.vs",
                    std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.fs");

    GLfloat axis_vertices[] = {
        // X 轴
        -10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 原点, 红色
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // X 轴端点, 红色
        // Y 轴
        0.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 原点, 绿色
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Y 轴端点, 绿色
        // Z 轴
        0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 1.0f,  // 原点, 蓝色
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f   // Z 轴端点, 蓝色
    };

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -10.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    //-------------------------------------
    // axis
    //-------------------------------------
    GLuint axis_vao, axis_vbo;
    glGenVertexArrays(1, &axis_vao);
    glGenBuffers(1, &axis_vbo);

    glBindVertexArray(axis_vao);

    glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), static_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //-------------------------------------------------------------------------
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attributes(s).
    //-------------------------------------------------------------------------
    unsigned int vao[2];
    unsigned int vbo;
    glGenVertexArrays(2, vao);
    glGenBuffers(1, &vbo);
    auto lighting_PosAttrib = glGetAttribLocation(lightingShader.prog_id(), "aPos");
    auto lighting_NormAttrib = glGetAttribLocation(lightingShader.prog_id(), "aNormal");
    auto lighting_TexCoordAttrib = glGetAttribLocation(lightingShader.prog_id(), "aTexCoords");
    auto lightCub_PosAttrib = glGetAttribLocation(lightCubeShader.prog_id(), "aPos");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //-------------------------------------
    // cube
    //-------------------------------------
    glBindVertexArray(vao[0]);
    glVertexAttribPointer(lighting_PosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lighting_PosAttrib);
    glVertexAttribPointer(lighting_NormAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(lighting_NormAttrib);
    glVertexAttribPointer(lighting_TexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(lighting_TexCoordAttrib);
    //-------------------------------------
    // lightCube
    //-------------------------------------
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Attributes
    glVertexAttribPointer(lightCub_PosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lightCub_PosAttrib);


    glEnable(GL_DEPTH_TEST);
    glLineWidth(5.0f);

    // light
    glm::vec3 lightPos(1.2f, 1.0f, 3.0f);
    glm::vec3 lightColor {1.0 };

    // load 
    unsigned int diffuseMap = loadTexture(std::filesystem::current_path() / "../../../../resource/textures/container2.png");
    unsigned int specularMap = loadTexture(std::filesystem::current_path() / "../../../../resource/textures/container2_specular.png");
    lightingShader.set("material.diffuse", 0);
    lightingShader.set("material.specular", 1);
    lightingShader.set("material.shininess", 32.0f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // 咋瓦鲁多
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view = camera.GetViewMatrix();
        auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);

        // axis
        {
            auto model = glm::mat4(1.0f);
            axisShader.use();
            axisShader.set("projection", projection);
            axisShader.set("view", view);
            axisShader.set("model", model);
            glBindVertexArray(axis_vao);
            glDrawArrays(GL_LINES, 0, 6);

        }
        // container
        {
            lightingShader.use();
            /*
               Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
               the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
               by defining light types as classes and set their values in there, or by using a more efficient uniform approach
               by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
            */
            // directional light
            lightingShader.set("dirLight.direction", glm::vec3{ -0.2f, -1.0f, -0.3f });
            lightingShader.set("dirLight.ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            lightingShader.set("dirLight.diffuse", glm::vec3{ 0.4f, 0.4f, 0.4f });
            lightingShader.set("dirLight.specular", glm::vec3{ 0.5f, 0.5f, 0.5f });
            // point light 1
            lightingShader.set("pointLights[0].position", pointLightPositions[0]);
            lightingShader.set("pointLights[0].ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            lightingShader.set("pointLights[0].diffuse", glm::vec3{ 0.8f, 0.8f, 0.8f });
            lightingShader.set("pointLights[0].specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("pointLights[0].constant", 1.0f);
            lightingShader.set("pointLights[0].linear", 0.09f);
            lightingShader.set("pointLights[0].quadratic", 0.032f);
            // point light 2
            lightingShader.set("pointLights[1].position", pointLightPositions[1]);
            lightingShader.set("pointLights[1].ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            lightingShader.set("pointLights[1].diffuse", glm::vec3{ 0.8f, 0.8f, 0.8f });
            lightingShader.set("pointLights[1].specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("pointLights[1].constant", 1.0f);
            lightingShader.set("pointLights[1].linear", 0.09f);
            lightingShader.set("pointLights[1].quadratic", 0.032f);
            // point light 3
            lightingShader.set("pointLights[2].position", pointLightPositions[2]);
            lightingShader.set("pointLights[2].ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            lightingShader.set("pointLights[2].diffuse", glm::vec3{ 0.8f, 0.8f, 0.8f });
            lightingShader.set("pointLights[2].specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("pointLights[2].constant", 1.0f);
            lightingShader.set("pointLights[2].linear", 0.09f);
            lightingShader.set("pointLights[2].quadratic", 0.032f);
            // point light 4
            lightingShader.set("pointLights[3].position", pointLightPositions[3]);
            lightingShader.set("pointLights[3].ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            lightingShader.set("pointLights[3].diffuse", glm::vec3{ 0.8f, 0.8f, 0.8f });
            lightingShader.set("pointLights[3].specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("pointLights[3].constant", 1.0f);
            lightingShader.set("pointLights[3].linear", 0.09f);
            lightingShader.set("pointLights[3].quadratic", 0.032f);
            // spotLight
            lightingShader.set("spotLight.position", camera.Position);
            lightingShader.set("spotLight.direction", camera.Front);
            lightingShader.set("spotLight.ambient", glm::vec3{ 0.0f, 0.0f, 0.0f });
            lightingShader.set("spotLight.diffuse", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("spotLight.specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            lightingShader.set("spotLight.constant", 1.0f);
            lightingShader.set("spotLight.linear", 0.09f);
            lightingShader.set("spotLight.quadratic", 0.032f);
            lightingShader.set("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            lightingShader.set("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            lightingShader.set("viewPos", camera.Position);
            lightingShader.set("projection", projection);
            lightingShader.set("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);
            glBindVertexArray(vao[0]);
            for (unsigned int i = 0; i < 10; i++)
            {
                glm::mat4 model{1.0f};
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.set("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        // lighting source
        {
            auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
            auto view = camera.GetViewMatrix();
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.3f)); // a smaller cube
            
            // also draw the lighting source
            lightCubeShader.use();
            lightCubeShader.set("projection", projection);
            lightCubeShader.set("view", view);
            lightCubeShader.set("cubeColor", lightColor);

            glBindVertexArray(vao[1]);
            for (auto&& pointLightPosition : pointLightPositions)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPosition);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightCubeShader.set("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

        }


        // Swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(const std::filesystem::path& path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    if (const auto data = ::stbi_load(path.generic_string().c_str(), &width, &height, &nrComponents, 0))
    {
        GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            assert(false);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << '\n';
        assert(false);
        stbi_image_free(data);
    }

    return textureID;
}
