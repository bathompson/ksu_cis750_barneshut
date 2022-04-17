#version 330 core

out vec4 FragColor;

varying float vMass;

uniform sampler2D massTexture;
uniform sampler2D starTexture;

void main() {
   vec4 massColor = texture(massTexture, vec2(vMass, 0.5));
   vec4 starColor = texture(starTexture, gl_PointCoord.st);
   massColor.a = starColor.r;
   FragColor = massColor;
}
