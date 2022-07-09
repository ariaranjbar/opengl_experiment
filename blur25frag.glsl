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
        float kernel[25] = float[](
            1.0f/273,  4.0f/273,  7.0f/273,  4.0f/273,  1.0f/273,
                                           
                                           
            4.0f/273, 16.0f/273, 26.0f/273, 16.0f/273,  4.0f/273,
                                           
                                           
            7.0f/273, 26.0f/273, 41.0f/273, 26.0f/273,  7.0f/273,
                                           
                                           
            4.0f/273, 16.0f/273, 26.0f/273, 16.0f/273,  4.0f/273,
                                           
                                           
            1.0f/273,  4.0f/273,  7.0f/273,  4.0f/273,  1.0f/273
        );

        vec2 offsets[25] = vec2[](
            vec2(-2*offset.x,  2*offset.y),
            vec2(-offset.x,  2*offset.y),
            vec2( 0.0f,    2*offset.y),
            vec2( offset.x,  2*offset.y),
            vec2( 2*offset.x,  2*offset.y),

            vec2(-2*offset.x,  offset.y),
            vec2(-offset.x,  offset.y),
            vec2( 0.0f,    offset.y),
            vec2( offset.x,  offset.y),
            vec2( 2*offset.x,  offset.y),

            vec2(-2*offset.x,  0.0f),
            vec2(-offset.x,  0.0f),
            vec2( 0.0f,    0.0f),
            vec2( offset.x, 0.0f),
            vec2( 2*offset.x,  0.0f), 

            vec2(-2*offset.x,  -offset.y),
            vec2(-offset.x,  -offset.y),
            vec2( 0.0f,    -offset.y),
            vec2( offset.x,  -offset.y),
            vec2( 2*offset.x,  -offset.y), 

            vec2(-2*offset.x,  -2*offset.y),
            vec2(-offset.x,  -2*offset.y),
            vec2( 0.0f,    -2*offset.y),
            vec2( offset.x,  -2*offset.y),
            vec2( 2*offset.x,  -2*offset.y)  
        );

        vec4 sampleTex[25];
        for(int i = 0; i < 25; i++)
        {
            vec4 tex = texture(u_frameBuffer, texCoord.st + offsets[i]);
            if(tex.r*tex.g*tex.b>0)
                sampleTex[i] = tex;
            else
                sampleTex[i] = texture(u_frameBuffer, texCoord.st + offsets[i]);
                sampleTex[i].a = tex.a;
        }

        vec4 col = vec4(0.0);
        for(int i = 0; i < 25; i++)
            col += sampleTex[i] * kernel[i];
        
        FragColor = col;
    }
} 