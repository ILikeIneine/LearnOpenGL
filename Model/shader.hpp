#pragma once
#include <cassert>
#include <concepts>
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:
    Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
        :ID(0u)
    {
        std::string vertex_code;
        std::string fragment_code;

        std::ifstream vertex_fs{ vertex_path};
        std::ifstream fragment_fs{ fragment_path};
        assert(exists(vertex_path) && exists(fragment_path));
        if (not vertex_fs || not fragment_fs)
        {
            throw std::ios_base::failure("file does not exist");
        }

        try
        {
            std::stringstream vertex_stream;
            std::stringstream fragment_stream;

            vertex_fs >> vertex_stream.rdbuf();
            fragment_fs >> fragment_stream.rdbuf();

            vertex_code = vertex_stream.str();
            fragment_code = fragment_stream.str();
        }
        catch (std::ifstream::failure&)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
        }

        auto vertex_shader = loadShader(GL_VERTEX_SHADER, vertex_code.data());
        auto fragment_shader = loadShader(GL_FRAGMENT_SHADER, fragment_code.data());

        ID = createProgramWithShaders(vertex_shader, fragment_shader);
    }
    ~Shader() = default;

    void use() const
    {
        if (ID)
        {
            glUseProgram(ID);
        }
        else
        {
            //   
        }
    }

    auto prog_id() const { return ID; }

    template <typename T>
    void set(const std::string& name, T&& value) const
    {
        use();
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, bool> || std::is_same_v<std::remove_cvref_t<T>, int>)
        {
            glUniform1i(glGetUniformLocation(ID, name.data()), static_cast<int>(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
        {
            glUniform1f(glGetUniformLocation(ID, name.data()), std::forward<T>(value));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec2>)
        {
            glUniform2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec3>)
        {
            glUniform3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec4>)
        {
            glUniform4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat2>)
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat3>)
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat4>)
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else
        {
            assert(false);
        }
    }



private:
    auto loadShader(GLenum shader_type, const char* shader_src) const
	-> GLuint
    {
        (void)this;
        const auto shader = glCreateShader(shader_type);
        if (shader == 0)
        {
            std::cout << "Failed to create shader!\n";
        }
        glShaderSource(shader, 1, &shader_src, nullptr);
        glCompileShader(shader);

        assert(checkCompileErrors(shader));
        return shader;
    }

    template<typename... Shaders>
        requires (std::same_as<std::invoke_result_t<decltype(glCreateShader), GLenum>, Shaders>&&...)
    auto createProgramWithShaders(Shaders ... shaders)
	-> GLuint
    {
        (void)this;
        // create program
        auto shader_program = glCreateProgram();
        // attach shader
        (glAttachShader(shader_program, shaders), ...);
        // link
        glLinkProgram(shader_program);
        // check linking result
        assert(checkCompileErrors(shader_program, "PROGRAM"));
        (glDeleteShader(shaders), ...);
        return shader_program;
    }

    bool checkCompileErrors(GLuint shader, std::string_view type_hint = "SHADER") const
    {
        (void)this;
        if (type_hint != "PROGRAM")
        {
	        int compile_status;
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
                return false;
	        }
        }
        else
        {
            int link_status;
            glGetProgramiv(shader, GL_LINK_STATUS, &link_status);
            // failed
            if (!link_status)
            {
                GLint info_len = 0;
                glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &info_len);
                if (info_len > 1)
                {
                    std::string info_log;
                    info_log.resize(info_len);
                    glGetProgramInfoLog(shader, info_len, nullptr, info_log.data());
                    std::cout << std::format("Failed to link shader, reason:\n{}", info_log);
                }
                glDeleteProgram(shader);
                return false;
            }
        }
        return true;
    }

private:
    GLuint ID;
};