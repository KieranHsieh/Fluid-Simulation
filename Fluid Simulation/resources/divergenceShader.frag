#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform sampler2DRect w;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

void main() {
    float vl = texture(w, vTexCoords - vec2(1.0, 0.0)).x;
    float vr = texture(w, vTexCoords + vec2(1.0, 0.0)).x;
    float vb = texture(w, vTexCoords - vec2(0.0, 1.0)).y;
    float vt = texture(w, vTexCoords + vec2(0.0, 1.0)).y;

    float divergence = hrdx * (vr - vl + vt - vb);
    fColor = vec4(divergence);
}