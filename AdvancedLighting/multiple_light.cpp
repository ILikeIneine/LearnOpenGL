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

#define PROJECT_NAME "AdvancedLighting"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const std::filesystem::path&);


// settings
constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 1600;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = SCR_WIDTH  / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera{ {-0.2f, 0.3f, 5.0} };
bool blinn = false;
bool blinnKeyPressed = false;

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
    Shader boxShader(
        shader_entity<GL_VERTEX_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/material.vs"},
        shader_entity<GL_FRAGMENT_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/material.fs"});
    Shader floorShader(
        shader_entity<GL_VERTEX_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/floor.vs"},
        shader_entity<GL_FRAGMENT_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/floor.fs"});
    Shader lightSrcShader(
        shader_entity<GL_VERTEX_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.vs"},
        shader_entity<GL_FRAGMENT_SHADER> { std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/light_cube.fs"});
    Shader axisShader(
        shader_entity<GL_VERTEX_SHADER> {std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.vs"},
        shader_entity<GL_FRAGMENT_SHADER> { std::filesystem::current_path() / "../../../../" PROJECT_NAME "/shaders/axis.fs"});

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

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
    };

    constexpr float planeVertices[] = {
        // positions
        // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode).
        // this will cause the floor texture to repeat)
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
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
    // cubes
    //-------------------------------------------------------------------------
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    auto lighting_PosAttrib = glGetAttribLocation(boxShader.prog_id(), "aPos");
    auto lighting_NormAttrib = glGetAttribLocation(boxShader.prog_id(), "aNormal");
    auto lighting_TexCoordAttrib = glGetAttribLocation(boxShader.prog_id(), "aTexCoords");
    auto lightCub_PosAttrib = glGetAttribLocation(lightSrcShader.prog_id(), "aPos");
    //-------------------------------------
    // box
    //-------------------------------------
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(lighting_PosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lighting_PosAttrib);
    glVertexAttribPointer(lighting_NormAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(lighting_NormAttrib);
    glVertexAttribPointer(lighting_TexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(lighting_TexCoordAttrib);
    //-------------------------------------
    // floor
    //-------------------------------------
    GLuint floor_vao, floor_vbo;
    glGenVertexArrays(1, &floor_vao);
    glGenBuffers(1, &floor_vbo);
    glBindVertexArray(floor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(lighting_PosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lighting_PosAttrib);
    glVertexAttribPointer(lighting_NormAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(lighting_NormAttrib);
    glVertexAttribPointer(lighting_TexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(lighting_TexCoordAttrib);
    //-------------------------------------
    // light src
    //-------------------------------------
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(lightCub_PosAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(lightCub_PosAttrib);

    //-------------------------------------------------------------------------
    // Texture 
    //-------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture(std::filesystem::current_path() / "../../../../resource/textures/container2.png");
    unsigned int specularMap = loadTexture(std::filesystem::current_path() / "../../../../resource/textures/container2_specular.png");
    unsigned int woodFloor = loadTexture(std::filesystem::current_path() / "../../../../resource/textures/wood.png");

    //-------------------------------------------------------------------------
    // global setting
    //-------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glLineWidth(5.0f);

    // light
    glm::vec3 lightPos(1.2f, 1.0f, 3.0f);
    glm::vec3 lightColor {1.0 };

    boxShader.set("material.diffuse", 0);
    boxShader.set("material.specular", 1);
    boxShader.set("material.shininess", 32.0f);

    floorShader.set("floorTexture", 0);

    //-------------------------------------------------------------------------
    // Game loop
    //-------------------------------------------------------------------------
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
            boxShader.use();
            /*
               Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
               the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
               by defining light types as classes and set their values in there, or by using a more efficient uniform approach
               by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
            */
            // directional light
            boxShader.set("dirLight.direction", glm::vec3{ -0.2f, -1.0f, -0.3f });
            boxShader.set("dirLight.ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            boxShader.set("dirLight.diffuse", glm::vec3{ 0.4f, 0.4f, 0.4f });
            boxShader.set("dirLight.specular", glm::vec3{ 0.5f, 0.5f, 0.5f });
            // point light 1
            boxShader.set("pointLights[0].position", pointLightPositions[0]);
            boxShader.set("pointLights[0].ambient", glm::vec3{ 0.05f, 0.05f, 0.05f });
            boxShader.set("pointLights[0].diffuse", glm::vec3{ 0.8f, 0.8f, 0.8f });
            boxShader.set("pointLights[0].specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            boxShader.set("pointLights[0].constant", 1.0f);
            boxShader.set("pointLights[0].linear", 0.09f);
            boxShader.set("pointLights[0].quadratic", 0.032f);
            // spotLight
            boxShader.set("spotLight.position", camera.Position);
            boxShader.set("spotLight.direction", camera.Front);
            boxShader.set("spotLight.ambient", glm::vec3{ 0.0f, 0.0f, 0.0f });
            boxShader.set("spotLight.diffuse", glm::vec3{ 1.0f, 1.0f, 1.0f });
            boxShader.set("spotLight.specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            boxShader.set("spotLight.constant", 1.0f);
            boxShader.set("spotLight.linear", 0.09f);
            boxShader.set("spotLight.quadratic", 0.032f);
            boxShader.set("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            boxShader.set("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            boxShader.set("viewPos", camera.Position);
            boxShader.set("projection", projection);
            boxShader.set("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            glBindVertexArray(cubeVAO);
            auto model = glm::mat4{ 1.0 };
            boxShader.set("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // floor
        {
            floorShader.use();
            floorShader.set("view", view);
            floorShader.set("projection", projection);
            floorShader.set("model", glm::mat4{1.0});

            floorShader.set("viewPos", camera.Position);
            floorShader.set("lightPos", pointLightPositions[0]);
            floorShader.set("blinn", blinn);

            glBindVertexArray(floor_vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodFloor);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // lighting source
        {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.3f)); // a smaller cube
            
            // also draw the lighting source
            lightSrcShader.use();
            lightSrcShader.set("projection", projection);
            lightSrcShader.set("view", view);
            lightSrcShader.set("cubeColor", lightColor);

            glBindVertexArray(lightVAO);
            for (auto&& pointLightPosition : pointLightPositions)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPosition);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightSrcShader.set("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

        }


        // Swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }
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
