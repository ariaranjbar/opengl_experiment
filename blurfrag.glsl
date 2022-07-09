#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;
uniform float u_gamma;

uniform float u_textureWeight;
uniform float u_frameBufferWeight;


uniform float u_yShift;
uniform float u_xShift;
uniform vec2 u_offset;

const float PI = 3.141592653589793238462;


void main()
{
    vec2 tc = texCoord * 1.0f;
    float tw = u_textureWeight;
    float fbw = u_frameBufferWeight;
    vec2 uv1 = u_offset + tc.st;
    vec2 uv2 = - u_offset + tc.st;
    
    uv1.x = (uv1.y>1 || uv1.y<0)?uv1.x+u_yShift:uv1.x;
    uv1.y = (uv1.x>1 || uv1.x<0)?uv1.y+u_xShift:uv1.y;
    uv2.x = (uv2.y>1 || uv2.y<0)?uv2.x+u_yShift:uv2.x;
    uv2.y = (uv2.x>1 || uv2.x<0)?uv2.y+u_xShift:uv2.y;

    vec4 color = 
    clamp(texture(u_texture, uv1), 0.0f, 1.0f) * tw +
    clamp(texture(u_texture, uv2), 0.0f, 1.0f) * tw + 
    clamp(texture(u_frameBuffer, tc.st), 0.0f, 1.0f) * fbw;
    if (color.x + color.y + color.z > 3)
    {
        color = vec4(1,0,0,1);
    }
    //vec4 color = 
    //texture(u_texture, uv1) * u_weight +
    //texture(u_texture, uv2) * u_weight + 
    //texture(u_texture, tc.st)*(1 - 2 * u_weight);

    FragColor = vec4(pow(color.xyz, vec3(1 / u_gamma)), color.w);
} 