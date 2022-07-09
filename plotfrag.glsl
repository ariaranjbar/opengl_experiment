#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;
uniform float u_gamma;
uniform float u_time;

const float PI = 3.141592653589793238462;

void main()
{
    const float pace = 0.5;
	float timeSin = sin(mod(u_time * pace, 1.0) * PI);
    float timeLoop = fract(u_time * pace);
    float x = abs(texCoord.s * 2 - 1);
    float y = abs(texCoord.t * 2 - 1);
    float v1 = texture(u_frameBuffer, vec2(x, 0.0f)).x;
    float v2 = texture(u_frameBuffer, vec2(y, 0.0f)).x;
    //vec3 color = vec3(smoothstep(0.995f, 0.999f, 1 - abs(v - y)));
    vec3 color = vec3 (smoothstep(0.995f, 0.999f, 1 - abs (timeLoop - v1 * v2))) + vec3 (v1 * v2, 0, 0);
    FragColor = vec4(pow(color, vec3(1/u_gamma)),1);
} 