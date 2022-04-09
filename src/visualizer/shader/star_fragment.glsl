R"(
#version 330 core

out vec4 FragColor;

varying float vMass;

void main() {
   FragColor = vec4(vMass / 5.0, vMass / 5.0, vMass / 5.0, 1.0f);
}
)"
