#pragma once

#include <type_traits>
#include <functional>
#include <glad/glad.h>
#include "vertex.h"

template <typename V>
class VertexBuffer {

public:
    VertexBuffer(const std::function<void(void)>& setAttribPointers);
    ~VertexBuffer();

    void bind(const V* data, size_t count) const;

private:
    unsigned int vao;
    unsigned int vbo;
};

template<typename V>
VertexBuffer<V>::VertexBuffer(const std::function<void(void)>& setAttribPointers) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    setAttribPointers();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

template<typename V>
VertexBuffer<V>::~VertexBuffer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

template<typename V>
void VertexBuffer<V>::bind(const V *data, size_t count) const {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V) * count, data, GL_DYNAMIC_DRAW);
}
