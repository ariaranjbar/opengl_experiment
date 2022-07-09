#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform ivec2 u_resolution;
uniform sampler2D u_env;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

//vec2 UVcoord(vec2 texCoord)
//{
//	vec2 uv;
//	uv.y = texCoord.y;
//	texCoord.x = texCoord.x * 2 - 1;
//	//float arc_slice = (2.0 * PI/u_resolution.x);
//	float y_angle = texCoord.y * PI;
//	float radius = sin(y_angle);
//	uv.x = (texCoord.x * radius) * 0.5 + 0.5;
//	return uv;
//}

//vec3 equirec(sampler2D map, vec2 texCoord)
//{
//	vec3 color;
//	float x, y;
//	x = texCoord.x;
//	y = (acos(-(texCoord.y*2-1))/PI); //true mollweide
//	//y = texCoord.y; //least distorted poles
//
//	x = x * sin(y*PI) + (0.5 * (1 - sin(y*PI))) ;
//
//	vec2 new_coords = vec2(x, texCoord.y);
//
//	color = texture(map, new_coords).xyz;
//
//	return color;
//}

vec3 equirec(sampler2D map, vec2 texCoord)
{
	vec3 color;
	float x, y;
	x = texCoord.x;
	y = texCoord.y;
	float x1, y1PI;
	y1PI = acos(-y * 2 + 1);
	x1 = 0.5 - sin(y1PI) * 0.5 + x * sin(y1PI);

	vec2 new_coords = vec2(x1, y);

	color = texture(map, new_coords).xyz;

	return color;
}

void main()
{
    FragColor = vec4(pow(equirec(u_env, texCoord), vec3(1/u_gamma)), 1);
} 