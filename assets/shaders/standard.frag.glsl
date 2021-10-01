#version 460 core

in VS_OUT
{
    vec3 vPosition;
    vec3 vNormal;
    vec2 vTexcoord;
}fs_in;

out vec4 fragColor;

void main()
{
    fragColor = vec4(fract(abs(fs_in.vPosition)), 1.0);
}