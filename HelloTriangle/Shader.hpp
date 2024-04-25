#pragma once
#include <concepts>
#include <iostream>
#include <format>
#include <glad/glad.h>

inline auto 
loadShader(GLenum shader_type, const char* shader_src) -> GLuint
{
    GLint compile_status;

    const auto shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_src, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (!compile_status)
    {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1)
        {
            std::string info_log;
            info_log.resize(info_len);
            glGetShaderInfoLog(shader, info_len, nullptr, info_log.data());
            std::cout << std::format("Failed to compile shader, reason: {}", info_log);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

template<typename... Shaders>
    requires (std::same_as<std::invoke_result_t<decltype(glCreateShader), GLenum>, Shaders>&&...)
auto 
linkShader(Shaders ... shaders) -> GLuint
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
            std::cout << std::format("Failed to link shader, reason: {}", info_log);
        }
        glDeleteProgram(shader_program);
        return 0;
    }
    // success
    (glDeleteShader(shaders), ...);

    return shader_program;
}