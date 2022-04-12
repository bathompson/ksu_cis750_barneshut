#version 330 core

out vec4 FragColor;

varying float vMass;

uniform sampler2D massTexture;

void main() {
   vec4 texVal = texture(massTexture, vec2(vMass, 0.5));
   FragColor = texVal;
}
