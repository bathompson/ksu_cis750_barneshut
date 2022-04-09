#include "glm/glm.hpp"

namespace StarPositionVertex
{
    static void setAttribPointers() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }
}

namespace StarMassVertex
{
    static void setAttribPointers() {
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    }
}
