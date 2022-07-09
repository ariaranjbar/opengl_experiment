#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_frameBuffer;
uniform sampler2D u_mask;
uniform ivec2 u_resolution;

void main()
{
    if (texture(u_mask, texCoord).r==0)
        FragColor = texture(u_frameBuffer, texCoord);
    else
    {
        vec2 offset = 1.0 / u_resolution; 
        float kernel[9] = float[](
            1.0/9.0, 1.0/9.0, 1.0/9.0,
            1.0/9.0, 1.0/9.0, 1.0/9.0,
            1.0/9.0, 1.0/9.0, 1.0/9.0
        );

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
        
        vec4 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = texture(u_frameBuffer, texCoord.st + offsets[i]);
        }
        vec4 col = vec4(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
        
        FragColor = col;
    }
} 