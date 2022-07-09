#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform ivec2 u_resolution;
uniform sampler2D u_env;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

vec3 moll(sampler2D map, vec2 texCoord)
{
	vec3 color;

	float x, y;
	x = texCoord.x;
	y = texCoord.y;
	
	float x1, y1PI;
	y1PI = acos(-y * 2 + 1);
	x1 = (x - 0.5 + sin(y1PI) * 0.5) * (1/sin(y1PI));

	vec2 new_coords = vec2(x1, y);

	color = texture(map, new_coords).xyz;

	return color;
}

void main()
{
	float x, y;
	x = texCoord.x * 2 - 1;
	y = texCoord.y * 2 - 1;

	vec3 borderColor;

	if(x > 0.0)
	{
		borderColor.x = 1;
	}
	else
	{
		borderColor.x = 0.3;
	}
	if(y > 0.0)
	{
		borderColor.y = 1;
	}
	else
	{
		borderColor.y = 0.3;
	}
	borderColor.z = 0.5;

	float radius = (x * x + y * y) - 1;
	float s = smoothstep(0.007, 0.01, radius);
	//vec3 color = borderColor * s + moll(u_env, texCoord) * (1-s);  //visualized border of true mollweide
	vec3 color = moll(u_env, texCoord);
    FragColor = vec4(pow(color, vec3(1/u_gamma)), 1);
} 