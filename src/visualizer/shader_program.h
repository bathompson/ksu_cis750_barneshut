#pragma once

#include <string>
#include <glm/glm.hpp>

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexCode, const std::string& fragmentCode);
    ~ShaderProgram();

    void use() const;

    void setUniformInt(const std::string& name, int value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, glm::vec2 value) const;
    void setUniformVec3(const std::string& name, glm::vec3 value) const;
    void setUniformVec4(const std::string& name, glm::vec4 value) const;
    void setUniformMat4(const std::string& name, glm::mat4 value) const;
private:
    unsigned int shaderProgram;
};
