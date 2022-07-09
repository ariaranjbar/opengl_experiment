#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;

out vec3 vertColor;

uniform mat4 u_modelMatrix;
uniform mat4 u_cameraMatrix;
uniform ivec2 u_resolution;
uniform vec2 u_offset;

void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	vec2 corner = vec2(- 1.0 + (50.0 / u_resolution.x),  1.0 - (50.0 / u_resolution.y));
	vec3 pos = (mat4(mat3(u_cameraMatrix)) * u_modelMatrix * vec4(aPos, 1.0)).xyz / 40;
	pos -= vec3(corner, 0.0);
	pos.xy += 2*u_offset;
	gl_Position = vec4(pos, 1.0);

	vertColor = aCol;
}
