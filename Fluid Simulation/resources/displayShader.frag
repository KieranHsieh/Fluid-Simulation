#version 440

layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform sampler2DRect tex;

void main() {
    fColor = texture(tex, vTexCoords);
}