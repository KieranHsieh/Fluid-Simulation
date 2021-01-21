#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform sampler2DRect vort;
uniform sampler2DRect u;
uniform vec2 dxscale;
uniform float timestep;
//uniform float epsilon;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

void main() {
    float vl = texture(vort, vTexCoords - vec2(1.0, 0.0)).x;
    float vr = texture(vort, vTexCoords + vec2(1.0, 0.0)).x;
    float vb = texture(vort, vTexCoords - vec2(0.0, 1.0)).x;
    float vt = texture(vort, vTexCoords + vec2(0.0, 1.0)).x;
    float vc = texture(vort, vTexCoords).x;

    vec2 force = hrdx * vec2(abs(vt) - abs(vb), abs(vr) - abs(vl));
    float lengthSquared = max(2.14414e-4, dot(force, force));
    force *= inversesqrt(lengthSquared) * dxscale * vc;
    force.y *= -1.0;

    vec2 velc = texture(u, vTexCoords).xy;
    fColor = vec4(velc + (timestep * force), 0.0, 1.0);
}