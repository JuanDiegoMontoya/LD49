#version 460 core

uniform mat4 u_invViewProj;
uniform vec3 u_viewPos;

in vec2 vTexcoord;

out vec4 fragColor;

bool intersectPlane(vec3 n, vec3 p0, vec3 l0, vec3 l, out float t) 
{ 
    float denom = dot(n, l);
    if (denom > 1e-6)
    {
        vec3 p0l0 = p0 - l0;
        t = dot(p0l0, n) / denom;
        return (t >= 0) ? true : false;
    }
 
    return false;
} 

vec3 Unproject(vec2 uv, mat4 invXProj)
{
  vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, 1.0, 1.0); // [0, 1] -> [-1, 1]

  // undo projection
  vec4 worldSpacePosition = invXProj * clipSpacePosition;
  worldSpacePosition /= worldSpacePosition.w;

  return worldSpacePosition.xyz;
}

void main()
{
    vec3 dir = normalize(Unproject(vTexcoord, u_invViewProj) - u_viewPos);
    fragColor = vec4(dir * 0.5 + 0.5, 1.0);

    float t_y = -1e7;
    if (abs(dir.y) > 1e-6)
        t_y = -u_viewPos.y / dir.y;

    //float t = 0;
    //if (intersectPlane(vec3(0, 1, 0), vec3(0), u_viewPos, dir, t))
    if (t_y >= 0.001)
    {
        vec3 hitp = u_viewPos + dir * t_y;
        hitp.y = 0;
        fragColor.xyz = vec3(fract(hitp));
    }
}