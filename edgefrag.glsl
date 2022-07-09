#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;

void main()
{
    vec2 offset = 0.2 / u_resolution;  
    vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y),
        vec2( 0.0f,    offset.y),
        vec2( offset.x,  offset.y),
        vec2(-offset.x,  0.0f), 
        vec2( 0.0f,    0.0f),   
        vec2( offset.x,  0.0f),  
        vec2(-offset.x, -offset.y),
        vec2( 0.0f,   -offset.y), 
        vec2( offset.x, -offset.y)     
    );

    float gx[9] = float[](
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    );

    float gy[9] = float[](
        1, 2, 1,
        0, 0, 0,
        -1, -2, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_frameBuffer, texCoord.st + offsets[i]));
    }
    vec3 gradX = vec3(0.0);
    for(int i = 0; i < 9; i++)
        gradX += sampleTex[i] * gx[i];
    vec3 gradY = vec3(0.0);
    for(int i = 0; i < 9; i++)
        gradY += sampleTex[i] * gy[i];
    vec3 mask = sqrt(pow(gradX, vec3(2)) + pow(gradY, vec3(2)));
    mask = step(.1, vec3(sqrt((mask.r + mask.g + mask.b)/3)));
    if (mask.r == 0)
        discard;

    FragColor = vec4(mask,1);
} 