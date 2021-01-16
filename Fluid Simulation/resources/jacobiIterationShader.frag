#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform float alpha;
uniform float rBeta;

uniform sampler2DRect x;
uniform sampler2DRect b;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

void main() {
    vec2 xL = texture(x, vTexCoords - vec2(1.0, 0.0)).xy;
    vec2 xR = texture(x, vTexCoords + vec2(1.0, 0.0)).xy;
    vec2 xB = texture(x, vTexCoords - vec2(0.0, 1.0)).xy;
    vec2 xT = texture(x, vTexCoords + vec2(0.0, 1.0)).xy;

    vec2 bC = texture(b, vTexCoords).xy;
    fColor = vec4((xL + xR + xB + xT + alpha * bC) * rBeta, 0.0, 0.0);
}