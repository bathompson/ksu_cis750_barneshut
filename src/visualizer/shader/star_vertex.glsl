#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aMass;

uniform mat4 view;
uniform mat4 projection;

varying float vMass;

void main() {
    gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_PointSize = 250.0 / gl_Position.z;
    vMass = aMass;
}
