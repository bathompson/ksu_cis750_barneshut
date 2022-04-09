#pragma once

#include <glad/glad.h>
#include "vertex.h"

template <typename V>
class VertexBuffer {

public:
    VertexBuffer();
    ~VertexBuffer();

    void bind(const V* data, size_t count) const;

private:
    unsigned int vbo;
};

template<typename V>
VertexBuffer<V>::VertexBuffer() {
    glGenBuffers(1, &vbo);
}

template<typename V>
VertexBuffer<V>::~VertexBuffer() {
    glDeleteBuffers(1, &vbo);
}

template<typename V>
void VertexBuffer<V>::bind(const V *data, size_t count) const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V) * count, data, GL_DYNAMIC_DRAW);
}
