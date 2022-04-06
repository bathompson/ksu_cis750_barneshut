#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_program.h"
#include "vertex_buffer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const std::string vs_source =
#include "shader/simple_vertex.glsl"
;

const std::string fs_source =
#include "shader/simple_fragment.glsl"
;

int main(int argc, char *argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    ShaderProgram shader = ShaderProgram(vs_source, fs_source);
    VertexBuffer<SimpleVertex> vbo = VertexBuffer<SimpleVertex>(SimpleVertex::setAttribPointers);

    SimpleVertex vertices[] = {
        {{-0.5f, -0.5f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}},
        {{0.0f,  0.5f, 0.0f}}
    };

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        vbo.bind(vertices, 3);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
