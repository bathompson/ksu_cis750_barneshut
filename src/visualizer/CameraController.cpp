#include "CameraController.h"

#include <GLFW/glfw3.h>

void CameraController::updateSize(int width, int height) {
    camera.width = width;
    camera.height = height;
    camera.update();
}

void CameraController::processInput(GLFWwindow *window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += speed * delta * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= speed * delta * camera.front;
    glm::vec3 right = glm::normalize(glm::cross(camera.front, camera.up));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * speed * delta;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * speed * delta;

    camera.update();
}

void CameraController::processMouse(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastMouseX;
        float yoffset = lastMouseY - ypos;
        lastMouseX = xpos;
        lastMouseY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera.front = glm::normalize(direction);
        camera.up = glm::normalize(glm::cross(glm::cross(camera.front, glm::vec3(0, 1, 0)), camera.front));

        camera.update();
    } else {
        firstMouse = true;
    }
}
