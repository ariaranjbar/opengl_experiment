#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 vertColor;
in vec3 normal;
in vec3 vertPos;

uniform vec3 u_lightColor;


void main()
{
	vec3 light_scaled = u_lightColor/max(max(u_lightColor.r, u_lightColor.b),u_lightColor.g);
	FragColor = vec4(light_scaled, 1.0);
}
