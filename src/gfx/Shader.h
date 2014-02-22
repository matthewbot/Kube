#ifndef TEST_SHADER_H
#define TEST_SHADER_H

#include "util/IDBase.h"

#include <glm/glm.hpp>

#include <initializer_list>
#include <functional>
#include <stdexcept>

class ShaderError : public std::runtime_error {
public:
    ShaderError(const std::string &msg);
};

class Shader : public IDBase<Shader> {
    friend class IDBase<Shader>;
public:
    enum class Type { VERTEX, FRAGMENT };

    Shader() { }
    Shader(Type type, const std::string &filename);

private:
    void deleteId();
};

class ShaderProgram : public IDBase<ShaderProgram> {
    friend class IDBase<ShaderProgram>;
public:    
    ShaderProgram() { }
    ShaderProgram(const std::initializer_list<std::reference_wrapper<const Shader>> &shaders);

    void setUniform(const std::string &name, const glm::mat4 &mat);

private:
    void deleteId();
};

#endif
