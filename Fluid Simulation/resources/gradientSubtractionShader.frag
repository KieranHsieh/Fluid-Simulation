#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform sampler2DRect p;
uniform sampler2DRect w;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};


void main() {
    float pL = texture(p, vTexCoords - vec2(1.0, 0.0)).x;
    float pR = texture(p, vTexCoords + vec2(1.0, 0.0)).x;
    float pB = texture(p, vTexCoords - vec2(0.0, 1.0)).x;
    float pT = texture(p, vTexCoords + vec2(0.0, 1.0)).x;
    
    fColor = texture(w, vTexCoords);
    fColor.xy -= hrdx * vec2(pR - pL, pT - pB);
}