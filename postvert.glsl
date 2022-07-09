#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTex;

out vec2 texCoord;

void main()
{
	
	gl_Position = vec4(aPos.x, -aPos.z, aPos.y , 1.0);
	texCoord = aTex;
}
