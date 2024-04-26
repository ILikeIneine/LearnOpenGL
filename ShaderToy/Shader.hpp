#pragma once
#include <concepts>
#include <iostream>
#include <format>

#include <glad/glad.h>

class Shader
{
public:
    Shader(std::string_view vertex_path, std::string_view fragment_path);
    ~Shader();

    void use() const;
    auto prog_id() const { return ID; }

    template <typename T>
    void set(const std::string& name, T value)
    {
	    if constexpr(std::is_same_v<T, bool> || std::is_same_v<T, int>)
	    {
            glUniform1i(glGetUniformLocation(ID, name.data()), static_cast<int>(value));
	    }
        else if constexpr (std::is_same_v<T, float>)
        {
            glUniform1f(glGetUniformLocation(ID, name.data()), value);
        }
    }

private:
    GLuint ID;
};

template<typename... Shaders>
    requires (std::same_as<std::invoke_result_t<decltype(glCreateShader), GLenum>, Shaders>&&...)
auto 
createProgramWithShaders(Shaders ... shaders) -> GLuint
{
    GLint link_result;
    // create program
    auto shader_program = glCreateProgram();
    // attach shader
    (glAttachShader(shader_program, shaders), ...);
    // link
    glLinkProgram(shader_program);
    // check linking result
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_result);
    // failed
    if (!link_result)
    {
        GLint info_len = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1)
        {
            std::string info_log;
            info_log.resize(info_len);
            glad_glGetProgramInfoLog(shader_program, info_len, nullptr, info_log.data());
            std::cout << std::format("Failed to link shader, reason:\n{}", info_log);
        }
        glDeleteProgram(shader_program);
        return 0;
    }
    // success
    (glDeleteShader(shaders), ...);
    return shader_program;
}