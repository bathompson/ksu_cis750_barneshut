#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
public:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 front = glm::vec3(0, 0, 0);
    glm::vec3 up = glm::vec3(0, 1, 0);

    float fov = glm::radians(45.0f);
    float near = 0.1f;
    float far = 1000.0f;
    int width = 800;
    int height = 600;

    Camera(glm::vec3 position, glm::vec3 front) : position(position), front(front) {
        update();
    }

private:
    glm::mat4 projMat = glm::mat4(1.0f);
    glm::mat4 viewMat = glm::mat4(1.0f);

public:
    void update() {
        projMat = glm::perspective(fov, (float)width/(float)height, near, far);
        viewMat = glm::lookAt(position, position + front, up);
    }
    glm::mat4 proj() const { return projMat; }
    glm::mat4 view() const { return viewMat; }
};
