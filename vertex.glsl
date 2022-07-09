#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec3 aNor;
layout(location = 3) in vec2 aTex;

out vec2 texCoord;
out vec3 vertColor;
out vec3 normal;
out vec3 vertPos;

uniform mat4 u_modelMatrix;
uniform mat4 u_cameraMatrix;
uniform vec2 u_offset;

void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	vec4 pos = u_cameraMatrix * u_modelMatrix * vec4(aPos, 1.0);
	pos.xy += 2*u_offset*pos.z;
	gl_Position = pos;
	texCoord = aTex;
	vertColor = aCol;
	normal = transpose(inverse(mat3(u_modelMatrix))) * aNor;
	vertPos = aPos;
}
