#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

uniform mat4 u_viewProj;
uniform mat4 u_model;

out VS_OUT
{
    vec3 vPosition;
    vec3 vNormal;
    vec2 vTexcoord;
}vs_out;

void main()
{
    vs_out.vPosition = (u_model * vec4(aPosition, 1.0)).xyz;
    vs_out.vNormal = (u_model * vec4(aNormal, 0.0)).xyz;
    vs_out.vTexcoord = aTexcoord;

    gl_Position = u_viewProj * vec4(vs_out.vPosition, 1.0);
}