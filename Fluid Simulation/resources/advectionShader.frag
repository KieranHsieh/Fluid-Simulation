#version 440 core
layout(location = 0) out vec4 fColor;

layout(location = 0) in vec2 vTexCoords;

uniform float timestep;
uniform float dissipation;
uniform sampler2DRect velocityTex;
uniform sampler2DRect targetTex;

layout(std140, binding = 0) uniform shaderData {
    vec2 gridDimensions;
    float rdx;
    float hrdx;
};

vec4 bilerp(sampler2DRect tex, vec2 uv)
{   
    vec4 st;
    st.xy = floor(uv - 0.5) + 0.5;
    st.zw = st.xy + 1.0;
  
    vec2 t = uv - st.xy; //interpolating factors 
    
    vec4 tex11 = texture(tex, st.xy);
    vec4 tex21 = texture(tex, st.zy);
    vec4 tex12 = texture(tex, st.xw);
    vec4 tex22 = texture(tex, st.zw);

    // bilinear interpolation
    return mix(mix(tex11, tex21, t.x), mix(tex12, tex22, t.x), t.y);
}


void main() {
    vec2 tc = vTexCoords - rdx * timestep * texture(velocityTex, vTexCoords).xy;
    fColor = dissipation * bilerp(targetTex, tc);
}