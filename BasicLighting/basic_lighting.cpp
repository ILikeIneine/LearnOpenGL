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

#define PROJECT_NAME "BasicLighting"

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

Camera camera{ {0.0f, 0.0f, 5.0} };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



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
    Shader lightingShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/gouraud.vs", 
                           std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/gouraud.fs");
    Shader lightCubeShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.vs",
                      std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.fs");
    Shader axisShader(std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.vs",
                    std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.fs");

    GLfloat axis_vertices[] = {
        // X 轴
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 原点, 红色
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // X 轴端点, 红色
        // Y 轴
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 原点, 绿色
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Y 轴端点, 绿色
        // Z 轴
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // 原点, 蓝色
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f   // Z 轴端点, 蓝色
    };

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
    auto lightCub_PosAttrib = glGetAttribLocation(lightCubeShader.prog_id(), "aPos");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //-------------------------------------
    // cube
    //-------------------------------------
    glBindVertexArray(vao[0]);
    glVertexAttribPointer(lighting_PosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lighting_PosAttrib);
    glVertexAttribPointer(lighting_NormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(lighting_NormAttrib);

    //-------------------------------------
    // lightCube
    //-------------------------------------
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Attributes
    glVertexAttribPointer(lightCub_PosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lightCub_PosAttrib);


    glEnable(GL_DEPTH_TEST);
    glLineWidth(5.0f);

    // lighting
    glm::vec3 lightPos(1.2f, 1.0f, 0.0f);
    glm::vec3 lightColor{ 0.0f, 1.0f, 1.0f };
    glm::vec3 objectColor{ 1.0f, 0.5f, 0.31f };

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
        lightPos.x = static_cast<float>(sin( glfwGetTime())) * 2.0f;
        lightPos.z = static_cast<float>(cos( glfwGetTime())) * 2.0f;

        // Draw 
        {
            auto model = glm::mat4(1.0f);
            auto view = camera.GetViewMatrix();
            auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
            axisShader.use();
            axisShader.set("projection", projection);
            axisShader.set("view", view);
            axisShader.set("model", model);

            glBindVertexArray(axis_vao);
            glDrawArrays(GL_LINES, 0, 6);

        }
        {
            auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
            auto view = camera.GetViewMatrix();
            auto model = glm::mat4(1.0f);

            lightingShader.use();
            lightingShader.set("objectColor", objectColor);
            lightingShader.set("lightColor", lightColor);
            lightingShader.set("lightPos", lightPos);
            lightingShader.set("projection", projection);
            lightingShader.set("viewPos", camera.Position);
            lightingShader.set("view", view);
            lightingShader.set("model", model);

            // render the cube
            glBindVertexArray(vao[0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
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
            lightCubeShader.set("model", model);

            glBindVertexArray(vao[1]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
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

