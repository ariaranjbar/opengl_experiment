#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform ivec2 u_resolution;
uniform sampler2D u_kernel;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

float kernval1();
float kernval2();

void main()
{
    float x = abs(texCoord.x * 2 - 1);
    float y = abs(texCoord.y * 2 - 1);
    float xval = texture(u_kernel, vec2(x, 0)).x;
    float yval = texture(u_kernel, vec2(y, 0)).x;
    vec3 color = vec3(kernval2());
    FragColor = vec4(pow(color, vec3(1/u_gamma)),1);
}

float kernval1()
{
    float x = abs(texCoord.x * 2 - 1);
    float y = abs(texCoord.y * 2 - 1);
    float xval = texture(u_kernel, vec2(x, 0)).x;
    float yval = texture(u_kernel, vec2(y, 0)).x;
    float val = (xval * yval);
    return val;
}

float kernval2()
{
    float x = abs(texCoord.x * 2 - 1);
    float y = abs(texCoord.y * 2 - 1);
    float xval = texture(u_kernel, vec2(x, 0)).x;
    float yval = texture(u_kernel, vec2(y, 0)).x;
    float diff = abs(xval - texCoord.y);
    float val = smoothstep(0.001, 0.01, diff);
    //float val = (xval * yval);
    return val;
}