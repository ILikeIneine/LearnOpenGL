#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "Shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    //******************************
    // window setup
    //******************************
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto window = glfwCreateWindow(800, 800, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    const auto triangle_vs = std::filesystem::current_path() / "../../../../ShaderToy/shaders/triangle.vs";
    const auto triangle_fs = std::filesystem::current_path() / "../../../../ShaderToy/shaders/triangle.fs";
    const auto axis_vs = std::filesystem::current_path() / "../../../../ShaderToy/shaders/axis.vs";
    const auto axis_fs = std::filesystem::current_path() / "../../../../ShaderToy/shaders/axis.fs";

	Shader triangleShader(triangle_vs, triangle_fs);
    Shader axisShader(axis_vs, axis_fs);
    constexpr float vertices[] = {
	    -0.6f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
	    0.6f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
	    0.0f, 0.73f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    constexpr unsigned int indices[] = {
		0,1,2
    };

    constexpr float axis[] = {
    -1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, 1.0, 0.0
    };

    constexpr unsigned int axis_indices[] = {
		0,1,
        2,3
    };

    //**************************************************************************
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attributes(s).
    //**************************************************************************
    unsigned int vao[2];
	unsigned int vbo[2];
    unsigned int ebo[2];
    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);
    glGenBuffers(2, ebo);
    auto posAttrib = glGetAttribLocation(triangleShader.prog_id(), "aPos");
    auto colorAttrib = glGetAttribLocation(triangleShader.prog_id(), "aColor");
    auto axisPosAttrib = glGetAttribLocation(axisShader.prog_id(), "aPos");
    //***************
    // Bind triangle
    //***************
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Attributes
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(colorAttrib);

    //***********
    // Bind axis
    //***********
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axis_indices), axis_indices, GL_STATIC_DRAW);
    // Attributes
    glVertexAttribPointer(axisPosAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(axisPosAttrib);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader program
        triangleShader.use();
        triangleShader.set("xOffset",float(sin(glfwGetTime()) / 2.0f + 0.5f));
        // do paint
        glBindVertexArray(vao[0]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        axisShader.use();
        glBindVertexArray(vao[1]);
        glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);

        // swap frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
    glDeleteVertexArrays(2,vao);
    glDeleteBuffers(2, vbo);

    glfwTerminate();
	return 0;
}