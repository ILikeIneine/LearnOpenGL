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

#define PROJECT_NAME "Instancing"

// settings
constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 1600;

float mixValue = 0.2f;


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
auto loadTexture(const std::filesystem::path&) -> GLuint;

auto genModelMatrices(std::size_t) -> std::vector<glm::mat4>;


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
    Shader defaultShader(
        shader_entity<GL_VERTEX_SHADER>{std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/shader.vs" },
        shader_entity<GL_FRAGMENT_SHADER >{std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/shader.fs"}
    );

    Shader asteroidShader(
        shader_entity<GL_VERTEX_SHADER>{std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/rock.vs" },
        shader_entity<GL_FRAGMENT_SHADER >{std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/rock.fs"}
    );

    //-------------------------------------
    // Initialize model
    //-------------------------------------
    Model planetModel { (std::filesystem::current_path() / "../../../../resource/planet/planet.obj").generic_string() };
    Model rockModel { (std::filesystem::current_path() / "../../../../resource/rock/rock.obj").generic_string() };
    int amount = 10'000;
    auto modelMatrices = genModelMatrices(amount);

    //-------------------------------------
    // VAO VBO EBO... and vertex attributes
    //-------------------------------------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * amount, modelMatrices.data(), GL_STATIC_DRAW);

    // overwrite the attribute
    for (auto&& mesh : rockModel.meshes)
    {
        auto vao = mesh.VAO;
        glBindVertexArray(vao);

        auto vec4_size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT,GL_FALSE, sizeof(glm::mat4), nullptr);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,sizeof(glm::mat4), reinterpret_cast<void*>(vec4_size));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE ,sizeof(glm::mat4), reinterpret_cast<void*>(2 * vec4_size));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(3 * vec4_size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    //--------------------------------------
    // global setting
    //--------------------------------------

    glEnable(GL_DEPTH_TEST);
    camera.MovementSpeed = 10.0f;

    //--------------------------------------
    // Main loop
    //--------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 咋瓦鲁多
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        defaultShader.use();
        defaultShader.set("view", view);
        defaultShader.set("projection", projection);
        asteroidShader.use();
        asteroidShader.set("view", view);
        asteroidShader.set("projection", projection);
        {
            defaultShader.use();
            glm::mat4 model{ 1.0f };
            model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
            defaultShader.set("model", model);
            planetModel.Draw(defaultShader);

        }
        {
            asteroidShader.use();
            asteroidShader.set("material1.diffuse", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rockModel.textures_loaded[0].id);
            for (auto& mesh : rockModel.meshes)
            {
                glBindVertexArray(mesh.VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0, amount);
                glBindVertexArray(0);
            }
        }

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

auto genModelMatrices(std::size_t amount) -> std::vector<glm::mat4>
{
    std::vector<glm::mat4> modelMatrices(amount, {1.0});
    srand(glfwGetTime());

    float radius = 50.0f;
    float offset = 2.5f;

    for (std::size_t i = 0; i < amount; ++i)
    {
        glm::mat4 model{1.0f};
        float angle = static_cast<float>(i) / static_cast<float>(amount) * 360.0f;

        // [-offset, offset]
        float displacement = rand() % static_cast<int>(2 * offset * 100) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = rand() % static_cast<int>(2 * offset * 100) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = rand() % static_cast<int>(2 * offset * 100) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3{ x,y,z });

        float scale = static_cast<float>(rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = static_cast<float>(rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3{ 0.4, 0.6, 0.8 });

        modelMatrices[i] = model;
    }

    return modelMatrices;
}

