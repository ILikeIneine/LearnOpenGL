#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "Shader.hpp"

auto vertexShaderSource = R"(
#version 330 core 
layout (location = 0) 
in vec3 aPos; 
void main()
{
   //Color = color;
   gl_Position = vec4(aPos, 1.0f);
}
)";

auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 triangleColor;
void main()
{
	
	FragColor = vec4(triangleColor, 1.0f);
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
    
    auto window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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
    auto shaderProgram = linkShader(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    float vertices[] = {
		-0.6f,0.0f,0.0f,
        -0.3f,0.4f,0.0f, 
        -0.01f,0.0f,0.0f,
    	0.01f,0.0f,0.0f, 
        0.3f,0.4f,0.0f,
        0.6f,0.0f,0.0f, 
    	-0.6f,-0.1f,0.0f,
        0.6f,-0.1f,0.0f, 
        0.0f, -0.4f, 0.0f
    };

    unsigned int indices[] = {
        0,1,2,
        3,4,5,
        6,7,8
    };


    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attributes(s).
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Specify the layout of the vertex data
    auto posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(posAttrib);
    //auto colAttrib = glGetAttribLocation(shaderProgram, "color");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);
    auto uniColor = glGetUniformLocation(shaderProgram, "triangleColor");

    // note that this is allowed, the call to glVertexAttribPointer 
    // registered VBO as the vertex attribute's bound vertex buffer 
    // object so afterward we can safely unbind
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterward so other VAO calls won't 
    // accidentally modify this VAO, but this rarely happens. 
    // Modifying other VAOs requires a call to glBindVertexArray 
    // anyway. So we generally don't unbind VAOs (nor VBOs) when 
    // it's not directly necessary.
    //glBindVertexArray(0);



    auto t_start = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        auto t_now = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        glUniform3f(uniColor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
	return 0;
}