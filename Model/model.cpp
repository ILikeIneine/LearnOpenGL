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
#include "model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// settings
constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 1200;

float mixValue = 0.2f;

// Non-changed
auto cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
auto cameraFront = glm::vec3(0.0, 0.0, -1.0);
auto cameraUp = glm::vec3(0.0, 1.0, 0.0);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera{ {0.0, 10.0, 20.0} };

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //----------------------------------------
    // glad: load all OpenGL function pointers
    //----------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }


    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //-------------------------------------
    // Create shader
    //-------------------------------------
    const auto model_vs = std::filesystem::current_path() / "../../../../Model/shaders/model.vs";
    const auto model_fs = std::filesystem::current_path() / "../../../../Model/shaders/model.fs";

    Shader modelShader(model_vs, model_fs);

    const auto model_path = std::filesystem::current_path() / "../../../../resource/nanosuit/nanosuit.obj";

    Model modelInstance{ model_path.generic_string() };


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // 咋瓦鲁多
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        modelShader.use();

        // view/projection transformations
        auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
        auto view = camera.GetViewMatrix();
        modelShader.set("view", view);
        modelShader.set("projection", projection);

        // render model
        auto model = glm::mat4{ 1.0f };
        model = glm::translate(model, glm::vec3{ 0.0f, 0.0f, 0.0f });
        model = glm::scale(model, glm::vec3{ 1.0f, 1.0f, 1.0f });
        modelShader.set("model", model);
        modelInstance.Draw(modelShader);



        // Swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

