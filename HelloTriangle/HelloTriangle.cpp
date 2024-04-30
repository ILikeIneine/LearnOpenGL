#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>

#include "Shader.hpp"

auto vertexShaderSource = R"(
#version 330 core 
layout (location = 0) 
in vec3 aPos; 
in vec3 color; 
uniform mat4 transform;
out vec3 Color;
void main()
{
   Color = color;
   gl_Position = transform * vec4(aPos, 1.0f);
}
)";

auto fragmentShaderSource = R"(
#version 330 core
in vec3 Color;
out vec4 FragColor;
void main()
{
	
	FragColor = vec4(Color, 1.0f);
}
)";


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
    // window setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    auto window = glfwCreateWindow(800, 800, "LearnOpenGL", nullptr, nullptr);
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

    // create vertex shader
    auto vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
    // create fragment shader
    auto fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    // link shaders
    auto shaderProgram = createProgramWithShaders(vertexShader, fragmentShader);


    const float first_triangle[] = {
	    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	    -0.6f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	    -0.6f, 0.6f, 0.0f, 1.0f, 0.0f, 0.0f,

	    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	    0.0f, 0.6f, 0.0f, 0.0f, 0.0f, 1.0f,
	    0.6f, 0.6f, 0.0f, 0.0f, 0.0f, 1.0f,

	    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	    0.6f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	    0.6f, -0.6f, 0.0f, 0.0f, 1.0f, 0.0f,

	    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	    0.0f, -0.6f, 0.0f, 1.0f, 1.0f, 0.0f,
	    -0.6f, -0.6f, 0.0f, 1.0f, 1.0f, 0.0f
    };

    const float second_triangle[] = {
	    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	    0.3f, 0.6f, 0.0f, 1.0f, 0.0f, 0.0f,
	    0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    const unsigned int indices1[] = {
        0,1,2,
        3,4,5,
        6,7,8,
        9,10,11
    };

    const unsigned int indices2[] = {
		0,1,2
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
    auto posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    auto colAttrib = glGetAttribLocation(shaderProgram, "color");

    // Bind first
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(first_triangle), first_triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    // Attributes
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);

    // Bind second
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(second_triangle), second_triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
    // Attributes
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        auto trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, -(float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, value_ptr(trans));

        glUseProgram(shaderProgram);
        glBindVertexArray(vao[0]);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
    glDeleteVertexArrays(2,vao);
    glDeleteBuffers(2, vbo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
	return 0;
}