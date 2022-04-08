#pragma once

#include "camera.h"

class GLFWwindow;

class CameraController {
private:
    Camera camera;

    bool firstMouse = true;
    double lastMouseX;
    double lastMouseY;

    float pitch = 0.0f;
    float yaw = -90.0f;

public:
    float speed = 2.5f;
    float sensitivity = 0.1f;

    CameraController(glm::vec3 position, glm::vec3 front) : camera(position, front) {}

    void updateSize(int width, int height);
    void processInput(GLFWwindow *window, float delta);
    void processMouse(GLFWwindow* window, double xpos, double ypos);

    glm::mat4 proj() { return camera.proj(); }
    glm::mat4 view() { return camera.view(); }
};
