#pragma once

#include <string>

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexCode, const std::string& fragmentCode);
    ~ShaderProgram();

    void use() const;
private:
    unsigned int shaderProgram;
};
