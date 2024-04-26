#include "Shader.hpp"

#include <complex.h>
#include <fstream>
#include <sstream>


auto
loadShader(GLenum shader_type, const char* shader_src)
-> GLuint
{
    GLint compile_status;

    const auto shader = glCreateShader(shader_type);
    if (shader == 0)
    {
        std::cout << "Failed to create shader!\n";
    }
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

Shader::Shader(std::string_view vertex_path, std::string_view fragment_path)
    :ID(0u)
{
    std::string vertex_code;
    std::string fragment_code;

    std::ifstream vertex_fs{ std::string(vertex_path)};
    std::ifstream fragment_fs{ std::string(fragment_path)};

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

Shader::~Shader()
{
    glDeleteShader(ID);
}


void Shader::use() const
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

