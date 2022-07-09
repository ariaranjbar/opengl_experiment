#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform ivec2 u_resolution;
uniform sampler2D u_frameBuffer;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

void main()
{
    vec3 color = texture(u_frameBuffer, texCoord.st).xyz;
    FragColor = vec4(pow(color, vec3(1/u_gamma)),1);
} 