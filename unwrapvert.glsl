#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec3 aNor;
layout(location = 3) in vec2 aTex;

out vec2 texCoords;
out vec3 vertColors;
out vec3 normals;

uniform mat4 u_modelMatrix;

//const float PI = 3.141592653589793238463;

void main()
{
	//const float pace = 0.05;
	//float timeSin = sin(mod(u_time * pace, 1.0) * PI);
	float a = 1;
	vec3 position = vec3(30*(aTex.x*2-1), 0.0, 30*(-aTex.y*2-1))*(1 - a) + aPos*a;
	gl_Position = vec4(position, 1.0);
	texCoords = aTex;
	vertColors = aCol;
	//normal = transpose(inverse(mat3(u_modelMatrix))) * aNor;
	normals = transpose(inverse(mat3(u_modelMatrix))) * (vec3(0, 1, 0) * (1 - a) + aNor * a);
}
