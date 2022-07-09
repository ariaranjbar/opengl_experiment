#version 330

layout(triangles) in;
layout(line_strip, max_vertices = 18) out;

in vec2[] texCoord;
in vec3[] normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_cameraMatrix;
uniform vec2 u_offset;

out vec3 color;

void main()
{
    
    vec3 avgpos = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3;

    vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    float a1 = texCoord[1].x - texCoord[0].x;
    float b1 = texCoord[1].y - texCoord[0].y;
    float a2 = texCoord[2].x - texCoord[0].x;
    float b2 = texCoord[2].y - texCoord[0].y;
    vec3 V = normalize((v1*a2 - v2*a1)/(b1*a2 - b2*a1));
    vec3 U = normalize((v1*b2 - v2*b1)/(a1*b2 - a2*b1));
    vec3 pos;
    vec3 Np;
    vec3 Vp;
    vec3 Up;
    vec3 Vf;
    vec3 Uf;
    vec3 n;
    float d;
    vec3 uv_average;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            pos = gl_in[j].gl_Position.xyz;
            pos += (avgpos - pos)/5;
            pos += normal[j]/240;
            Np = normal[j];
            Vp = cross(Np,cross(V, Np));
            Up = cross(Np,cross(U, Np));
            d = dot(cross(Up, Np), Vp);
            uv_average = normalize(Vp + Up);
            if(d>0)
            {
                Vf = normalize(cross(uv_average, Np) + uv_average);
                Uf = cross(Np, Vf);
            }
            else
            {
                Uf = normalize(cross(uv_average, Np) + uv_average);
                Vf = cross(Np, Uf);
            }
            switch (i) {
            case 0:
                n = Uf;
                break;
            case 1:
                n = Vf;
                break;
            case 2:
                n = Np;
                break;
            }
            switch (i) {
            case 0:
                color = vec3(1,0,0);
                break;
            case 1:
                color = vec3(0,1,0);
                break;
            case 2:
                color = vec3(0,0,1);
                break;
            }
            gl_Position = u_cameraMatrix * u_modelMatrix * vec4(pos, 1);
            gl_Position.xy += u_offset*gl_Position.z;
            EmitVertex();

            gl_Position = u_cameraMatrix * u_modelMatrix * vec4(pos + n/5, 1);
            gl_Position.xy += u_offset*gl_Position.z;
            EmitVertex();
            
            EndPrimitive();
        }
    }
}