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
    vec4 ul = texture(u, vTexCoords - vec2(1.0, 0.0));
    vec4 ur = texture(u, vTexCoords + vec2(1.0, 0.0));
    vec4 ub = texture(u, vTexCoords - vec2(0.0, 1.0));
    vec4 ut = texture(u, vTexCoords + vec2(0.0, 1.0));
  
    float vort = hrdx * ((ur.y - ul.y) - (ut.x - ub.x));
    fColor = vec4(vort);
}