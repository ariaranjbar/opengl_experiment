#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2[] texCoords;
in vec3[] vertColors;
in vec3[] normals;

out vec2 texCoord;
out vec3 vertColor;
out vec3 normal;
out vec3 vertPos;
out vec3 vertInd;
out mat3 TBN;

uniform mat4 u_modelMatrix;
uniform mat4 u_cameraMatrix;
uniform vec2 u_offset;

void main()
{

    vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    float a1 = texCoords[1].x - texCoords[0].x;
    float b1 = texCoords[1].y - texCoords[0].y;
    float a2 = texCoords[2].x - texCoords[0].x;
    float b2 = texCoords[2].y - texCoords[0].y;
    vec3 V = normalize((v1*a2 - v2*a1)/(b1*a2 - b2*a1));
    vec3 U = normalize((v1*b2 - v2*b1)/(a1*b2 - a2*b1));

    vec3 N;
    vec3 Vp;
    vec3 Up;
    vec3 B;
    vec3 T;
    float d;
    vec3 uv_average;
    for (int i = 0; i <3; i++)
    {
        N = normals[i];
        Vp = cross(N,cross(V, N));
        Up = cross(N,cross(U, N));
        d = dot(cross(Up, N), Vp);
        uv_average = normalize(Vp + Up);
        if(d>0)
        {
            B = normalize(cross(uv_average, N) + uv_average);
            T = cross(N, B);
        }
        else
        {
            T = normalize(cross(uv_average, N) + uv_average);
            B = cross(N, T);
        }
        TBN = mat3(T,B,N);
        gl_Position = u_cameraMatrix * u_modelMatrix * gl_in[i].gl_Position;
        gl_Position.xy += 2*u_offset*gl_Position.z;
        switch (i) {
        case 0:
            vertInd = vec3(1,0,0);
            break;
        case 1:
            vertInd = vec3(0,1,0);
            break;
        case 2:
            vertInd = vec3(0,0,1);
            break;
        }
        texCoord = texCoords[i];
        vertColor = vertColors[i];
        normal = normals[i];
        vertPos = gl_in[i].gl_Position.xyz;
        EmitVertex();
    }
    EndPrimitive();
}