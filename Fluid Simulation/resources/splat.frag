#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;


uniform vec3 color;
uniform vec2 position;
uniform float radius;
uniform sampler2DRect base;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

float gaussian(vec2 pos, float radius) {
    return exp(-dot(pos, pos) / radius);
}

void main() {
    vec2 pos = gridDimensions * position - vTexCoords;
    float rad = gridDimensions.x * radius;

    fColor = texture(base, vTexCoords) + color.xyzz * gaussian(pos, rad);
}