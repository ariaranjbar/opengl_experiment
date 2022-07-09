#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

void main()
{
	vec3 color;
	float D;
    vec2 uv = texCoord;
	float a2 = uv.y * uv.y;
	a2 *= a2;
	float c2a;
	float c2b;
	float c2;
	float ss;
	c2a = cos((uv.x) * PI / 2);
	c2a = (c2a* c2a);
	c2b = (uv.x) * PI / 2;
	c2b = 1 - c2b * c2b;
	ss = smoothstep(0.04, 0.08, uv.x);
	c2 = ss * c2a + (1-ss) * c2b;
	D = a2/(PI * (c2 * (a2 - 1.0) + 1.0) * (c2 * (a2 - 1.0) + 1.0));
	color = vec3(D);
    FragColor = vec4(pow(color, vec3(1/u_gamma)),1);
} 