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

template <int I = 0>
struct shader_entity
{
    using Path = std::filesystem::path;

    Path path;
    static constexpr auto type = I;
};

class Shader
{
public:
    template<typename ... entity_types>
    requires requires { (std::convertible_to<entity_types, shader_entity<>>,...); }
    constexpr Shader(entity_types&&... shaders)
    {
        id_ = createProgramWithShaders(loadShader(std::forward<entity_types>(shaders))...);
    }
    ~Shader() = default;

    constexpr void use() const
    {
        if (id_)
        {
            glUseProgram(id_);
        }
        else
        {
            //   
        }
    }

    [[nodiscard]] constexpr auto prog_id() const { return id_; }

    template <typename T>
    void set(const std::string& name, T&& value) const
    {
        use();
        if constexpr (std::integral<std::remove_cvref_t<T>>)
        {
            glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
        {
            glUniform1f(glGetUniformLocation(id_, name.data()), std::forward<T>(value));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec2>)
        {
            glUniform2fv(glGetUniformLocation(id_, name.data()), 1, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec3>)
        {
            glUniform3fv(glGetUniformLocation(id_, name.data()), 1, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::vec4>)
        {
            glUniform4fv(glGetUniformLocation(id_, name.data()), 1, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat2>)
        {
            glUniformMatrix2fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat3>)
        {
            glUniformMatrix3fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, glm::mat4>)
        {
            glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
        }
        else
        {
            assert(false);
        }
    }

    constexpr operator GLuint() const
    {
        return prog_id();
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

    template<GLint I>
    auto loadShader(const shader_entity<I>& shader) const
        -> GLuint
    {
        std::string file_content;
        std::ifstream shader_stream{ shader.path };
        if (not shader_stream)
        {
            assert(false);
            throw std::ios_base::failure("file does not exist");
        }
        try
        {
            std::stringstream _ss;
            shader_stream >> _ss.rdbuf();
            file_content = _ss.str();
        }
        catch (std::ifstream::failure&)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
        }
        return loadShader(shader.type, file_content.c_str());
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
    GLuint id_{0u};
};