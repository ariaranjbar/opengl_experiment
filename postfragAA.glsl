#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform int u_count;
uniform sampler2D u_frameBuffer0;
uniform sampler2D u_frameBuffer1;
//uniform sampler2D u_frameBuffer2;
//uniform sampler2D u_frameBuffer3;
//uniform sampler2D u_frameBuffer4;
//uniform sampler2D u_frameBuffer5;
//uniform sampler2D u_frameBuffer6;
//uniform sampler2D u_frameBuffer7;
//uniform sampler2D u_frameBuffer8;
uniform ivec2 u_resolution;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

void main()
{
    vec3 frame;
    frame = texture(u_frameBuffer0, texCoord.st).xyz*(1.0/u_count);
    frame += texture(u_frameBuffer1, texCoord.st).xyz*(1-(1.0/u_count));
    //frame += texture(u_frameBuffer2, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer3, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer4, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer5, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer6, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer7, texCoord.st).xyz/count;
    //frame += texture(u_frameBuffer8, texCoord.st).xyz/count;

    FragColor = vec4(pow(frame, vec3(1/u_gamma)),1);
} 