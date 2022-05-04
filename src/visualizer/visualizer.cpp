#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "shader_program.h"
#include "vertex_buffer.h"
#include "camera_controller.h"
#include "result_file.h"
#include "imgui_ui.h"
#include "texture.h"
#include <memory>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(resources);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

CameraController camera = CameraController(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1.0));
int screenWidth, screenHeight;

int main(int argc, char *argv[]) {
    auto fs = cmrc::resources::get_filesystem();

    std::string vs_source = fs.open("shader/star_vertex.glsl").begin();
    std::string fs_source = fs.open("shader/star_fragment.glsl").begin();

    glfwInit();
    glfwWindowHint(GLFW_DOUBLEBUFFER , 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "N-Body Simulation", nullptr, nullptr);
    screenWidth = 800;
    screenHeight = 600;
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    auto massTexFile = fs.open("textures/massgradient.png");
    Texture massTexture = Texture(massTexFile.begin(), massTexFile.end(), 0);

    auto starTexFile = fs.open("textures/light.png");
    Texture starTexture = Texture(starTexFile.begin(), starTexFile.end(), 1);

    ShaderProgram shader = ShaderProgram(vs_source, fs_source);
    VertexBuffer<float> positionVbo = VertexBuffer<float>();
    VertexBuffer<float> massVbo = VertexBuffer<float>();

    std::shared_ptr<ResultFile> results = nullptr;

    ImguiUI ui = ImguiUI(window);

    constexpr float SEEK_TIME_GAP = 0.2f;
    float timeSinceSeek = 0;

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    glEnable(GL_PROGRAM_POINT_SIZE);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float timeSinceChange = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.processInput(window, deltaTime);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ui.sceneFPS = 1.0f / deltaTime;

        if (ui.hasNewFile()) {
            results = std::make_shared<ResultFile>(ui.popFile(), true);
            ui.setMaxFrame(results->count_frames());
        }

        if (results != nullptr) {
            if (!ui.playing) {
                results->seek(ui.selectedFrame);
            } else {
                timeSinceChange += deltaTime;
                if (timeSinceChange > (1.0f / ui.playbackFPS)) {
                    if (results->current_frame() + 1 >= results->count_frames()) {
                        results->seek(0);
                    } else {
                        results->seek(results->current_frame() + 1);
                    }
                    timeSinceChange = 0;
                }
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            glBindVertexArray(vao);
            positionVbo.bind(results->positions(), results->num_bodies() * 3);
            StarPositionVertex::setAttribPointers();
            glEnableVertexAttribArray(0);
            massVbo.bind(results->masses(), results->num_bodies());
            StarMassVertex::setAttribPointers();
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);

            shader.use();
            shader.setUniformInt("massTexture", 0);
            shader.setUniformInt("starTexture", 1);
            shader.setUniformMat4("projection", camera.proj());
            shader.setUniformMat4("view", camera.view());
            shader.setUniformFloat("screenScale", screenHeight / 1080.0f);

            glBindVertexArray(vao);
            glDrawArrays(GL_POINTS, 0, results->num_bodies());

            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUseProgram(0);
        }

        ui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    camera.updateSize(width, height);
    screenWidth = width;
    screenHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouse(window, xpos, ypos);
}
