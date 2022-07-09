#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec3 aNor;
layout(location = 3) in vec2 aTex;

out vec2 texCoords;
out vec3 vertColors;
out vec3 normals;

uniform mat4 u_modelMatrix;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	texCoords = aTex;
	vertColors = aCol;
	normals = transpose(inverse(mat3(u_modelMatrix))) * aNor;;
}
