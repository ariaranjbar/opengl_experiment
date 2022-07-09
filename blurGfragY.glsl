#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;
uniform float u_gamma;


uniform float u_frameBufferWeight;

const int kernelsize = 20;

uniform float u_textureWeights[kernelsize];

uniform float u_offsets[kernelsize];

const float PI = 3.141592653589793238462;


void main()
{
    float fbw = u_frameBufferWeight;
    vec2 tc = texCoord * 1.0f;

    vec2 uv1[kernelsize];
    vec2 uv2[kernelsize];
    for(int i = 0; i < kernelsize; i++)
    {
        uv1[i] = vec2(tc.x,  u_offsets[i] + tc.y);
        uv2[i] = vec2(tc.x, -u_offsets[i] + tc.y);
    }
    
    for(int i = 0; i < kernelsize; i++)
    {
        if (mod(uv1[i].y, 2) > 1)
        {
            uv1[i] = vec2(fract(uv1[i].x + 0.5), 1 - fract(uv1[i].y));
        }
        else
        {
            uv1[i] = fract(uv1[i]);
        }
        if (mod(uv2[i].y, 2) > 1)
        {
            uv2[i] = vec2(fract(uv2[i].x + 0.5), 1 - fract(uv2[i].y));
        }
        else
        {
            uv2[i] = fract(uv2[i]);
        }
    }

    vec4 color = clamp(texture(u_frameBuffer, tc.st), 0.0f, 100.0f) * fbw;

    for(int i = 0; i < kernelsize; i++)
    {
        color += 
        clamp(texture(u_texture, uv1[i]), 0.0f, 100.0f) * u_textureWeights[i] +
        clamp(texture(u_texture, uv2[i]), 0.0f, 100.0f) * u_textureWeights[i];
    }

    //if (color.x + color.y + color.z > 3)
    //{
    //    color = vec4(1,0,0,1);
    //}
    //vec4 color = 
    //texture(u_texture, uv1) * u_weight +
    //texture(u_texture, uv2) * u_weight + 
    //texture(u_texture, tc.st)*(1 - 2 * u_weight);

    FragColor = vec4(pow(color.xyz, vec3(1 / u_gamma)), color.w);
} 