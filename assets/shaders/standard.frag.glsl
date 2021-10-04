#version 460 core

uniform vec3 u_sunDir;
uniform float u_blendDay;
uniform vec4 u_color;
uniform vec3 u_glow;

in VS_OUT
{
    vec3 vPosition;
    vec3 vNormal;
    vec2 vTexcoord;
}fs_in;

out vec4 fragColor;

vec3 faceNormal(vec3 wPos)
{
    return normalize(cross(dFdx(wPos), dFdy(wPos)));
}

vec4 GetDiffuse()
{
    return u_color;
}

void main()
{
    vec3 sunDay = vec3(1);
    vec3 sunNight = vec3(0.3);
    vec3 sun = mix(sunNight, sunDay, u_blendDay);

    vec3 N = faceNormal(fs_in.vPosition);
    float NoL = max(0.0, dot(N, -u_sunDir));
    
    vec3 diffuse = GetDiffuse().rgb;

    vec3 sunLit = u_blendDay * diffuse * NoL * sun + sun * 0.1;

    vec3 groundColor = vec3(125.0 / 255, 46.0 / 255, 30.0 / 255);
    float groundDot = clamp(dot(-N, vec3(0, 1, 0)) + .3, 0.0, 1.0);
    vec3 groundLit = groundColor * groundDot;

    vec3 lit = sunLit + groundLit;

    if (u_color.a < 0.01) discard;
    fragColor = vec4(u_glow + lit + (0.06 * (N * 0.5 + 0.5)), u_color.a);
}