#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;


layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

uniform sampler2DRect u;

void main() {
    float vl = texture(u, vTexCoords - vec2(1.0, 0.0)).y;
    float vr = texture(u, vTexCoords + vec2(1.0, 0.0)).y;
    float vb = texture(u, vTexCoords - vec2(0.0, 1.0)).x;
    float vt = texture(u, vTexCoords + vec2(0.0, 1.0)).x;

    fColor = vec4(hrdx * (vr - vl - vt + vb), 0.0, 0.0, 1.0);
}