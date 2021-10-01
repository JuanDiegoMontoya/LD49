#version 460 core

vec3 tri[3] = {
    vec3(-1, -1, 0),
    vec3(1, -1, 0),
    vec3(0, 1, 0),
};

void main()
{
    gl_Position = vec4(tri[gl_VertexID], 1.0);
}