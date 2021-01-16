#version 440 core
layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTextureCoordinates;

layout(location = 0) out vec2 vTexCoords;

void main() {
    vTexCoords = vTextureCoordinates;
    gl_Position = vec4(vPosition, 0.0, 1.0);
}