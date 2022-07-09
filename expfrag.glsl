#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_frameBuffer;
uniform ivec2 u_resolution;
uniform float u_gamma;

const float PI = 3.141592653589793238462;

float calc_height(vec2 cooord);

void main()
{
    vec2 leftcoord = texCoord - vec2(0.5/u_resolution.x, 0);
    vec2 downcoord = texCoord - vec2(0, 0.5/u_resolution.y); 
    float val = calc_height(texCoord);
    float lval = calc_height(leftcoord);
    float dval = calc_height(downcoord);
    float dU = (lval - val);
    float dV = (dval - val);
    float a = max(abs(dU), abs(dV));
    float b = min(abs(dU), abs(dV));
    float r = b/a;
    r = atan(r);
    r = cos(r);
    vec2 uv = vec2(dU, dV);
    uv = uv/r;
    float dN = sqrt(1 - length(uv))/pow(u_gamma,4);
    vec3 col = normalize(vec3(uv, dN)) * 0.5 + 0.5;
    vec3 bw = vec3(val);
    FragColor = vec4(bw,1);
    //FragColor = vec4(pow(min(col1,col2)*col, vec3(1/u_gamma)),1);
} 

float calc_height(vec2 cooord)
{
    vec3 col1 = vec3(pow(cos((cooord.y+cooord.x)*10*PI)*0.5+0.5,0.05));
    vec3 col2 = vec3(pow(cos((cooord.y-cooord.x)*10*PI)*0.5+0.5,0.05));
    vec3 col = vec3(pow(cos(cooord.y*10*PI)*cos(texCoord.x*10*PI)*0.5+0.5,0.1));
    return col1*col2*col;
}