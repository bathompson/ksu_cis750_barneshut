#include "glm/glm.hpp"

struct SimpleVertex {
    glm::vec3 pos;

    static void setAttribPointers() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }
};
