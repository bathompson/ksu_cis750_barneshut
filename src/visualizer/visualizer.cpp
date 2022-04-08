#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "shader_program.h"
#include "vertex_buffer.h"
#include "CameraController.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const std::string vs_source =
#include "shader/simple_vertex.glsl"
;

const std::string fs_source =
#include "shader/simple_fragment.glsl"
;

CameraController camera = CameraController(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1.0));

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
    glfwSetCursorPosCallback(window, mouse_callback);

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

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setUniformMat4("projection", camera.proj());
        shader.setUniformMat4("view", camera.view());
        vbo.bind(vertices, 3);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    camera.updateSize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouse(window, xpos, ypos);
}
