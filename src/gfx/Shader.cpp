#include "gfx/Shader.h"
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>

ShaderError::ShaderError(const std::string &msg) : std::runtime_error(msg) { }

static std::string readfile(const std::string &filename);

Shader::Shader(Type type, const std::string &filename) {
    std::string file = readfile(filename);

    id = glCreateShader(type == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    const char *filecstr = file.c_str();
    glShaderSource(id, 1, &filecstr, NULL);

    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::unique_ptr<char []> log(new char [length+1]);
        glGetShaderInfoLog(id, length, NULL, log.get());
        log[length] = '\0';

        std::stringstream buf;
        buf << "Compile failure in " << filename << " shader:" << std::endl << log.get() << std::endl;
        throw ShaderError(buf.str());
    }
}

void Shader::deleteId() {
    glDeleteShader(id);
}

ShaderProgram::ShaderProgram(const std::initializer_list<std::reference_wrapper<const Shader>> &shaders) {
    id = glCreateProgram();
    for (const Shader &shader : shaders)
        glAttachShader(id, shader.getID());
    glLinkProgram(id);
    for (const Shader &shader : shaders)
        glDetachShader(id, shader.getID());

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

        std::unique_ptr<char []> log(new char [length+1]);
        glGetProgramInfoLog(id, length, NULL, log.get());
        log[length] = '\0';

        std::stringstream buf;
        buf << "Linking error for shader program:" << std::endl << log.get() << std::endl;
        throw ShaderError(buf.str());
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::mat4 &mat) {
    auto loc = glGetUniformLocation(id, name.c_str());
    glUseProgram(id);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::deleteId() {
    glDeleteShader(id);
}

static std::string readfile(const std::string &filename) {
    std::ifstream in(filename.c_str(), std::ifstream::in | std::ifstream::binary);
    in.seekg(0, std::ifstream::end);
    unsigned int length = in.tellg();
    in.seekg(0, std::ifstream::beg);

    std::unique_ptr<char []> buf(new char [length+1]);
    in.read(buf.get(), length);
    buf[length] = '\0';

    return buf.get();
}
