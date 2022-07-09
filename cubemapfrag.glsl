#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform ivec2 u_resolution;
uniform sampler2D u_env;
uniform int u_face;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

vec2 faceUVs(vec2 texCoord, int face)
{
    vec3 xyz;
    switch(face)
    {
    case 0:
        xyz.z = 1;
        xyz.y = texCoord.y * 2 - 1;
        xyz.x = -(texCoord.x * 2 - 1);
        break;
    case 1:
        xyz.z = -1;
        xyz.xy = texCoord * 2 - 1;
        break;
    case 2:
        xyz.y = -1;
        xyz.z = (texCoord.x * 2 - 1);
        xyz.x = (texCoord.y * 2 - 1);
        break;
    case 3:
        xyz.y = 1;
        xyz.z = texCoord.x * 2 - 1;
        xyz.x = -(texCoord.y * 2 - 1);
        break;
    case 4:
        xyz.x = 1;
        xyz.zy = texCoord * 2 - 1;
        break;
    case 5:
        xyz.x = -1;
        xyz.z = -(texCoord.x * 2 - 1);
        xyz.y = texCoord.y * 2 - 1;
        break;
    }
    vec3 ref = normalize(xyz);
	float v = (asin(ref.y)/(PI) + 0.5);
	ref.y = 0;
	ref = normalize(ref);
	float u = acos(dot(ref, vec3(1,0,0)))/(PI);
	float s = -sign(dot(ref, vec3(0,0,1)));
	u = u * s;
	u = u * 0.5 + 0.5;
    vec2 uv = vec2(u, v);
    return uv;
}

void main()
{
    FragColor = vec4(pow(texture(u_env, faceUVs(texCoord, u_face)).xyz  , vec3(1/u_gamma)), 1);
} 