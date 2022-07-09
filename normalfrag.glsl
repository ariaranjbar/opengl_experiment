#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 vertColor;
in vec3 normal;
in vec3 vertPos;

uniform sampler2D u_texture1;
uniform ivec2 u_resolution;
uniform float u_time;
uniform vec3 u_cameraPos;

const float PI = 3.141592653589793238463; 

float smoothstep(float edge, float value, float softness)
{
	float s = softness*0.5;
	float v = (value - edge)/s;
	return clamp(v, 0.0, 1.0);
}

float circle(vec2 position, float radius, float softness)
{
	return 1 - smoothstep(radius, length(position), softness);
}

float rectangle(vec2 position, vec2 scale)
{
	vec2 bottomleft = step(-scale, position);
	vec2 topright = 1.0 - step(scale, position);
	return bottomleft.x * bottomleft.y * topright.x * topright.y;
}

float polygon(vec2 position, float radius, int sides, float softness)
{
	float slice = (2.0 * PI) / sides;
	float angle = atan(position.y, position.x);
	float angleInSection = mod(angle, (2.0 * PI) / sides);
	float m = -1.0 / tan(slice/2.0);
	float n = tan(angleInSection);
	float x = m/(m-n);
	float y = m*n/(m-n);
	float r = length(vec2(x, y));
	return 1- smoothstep(radius*r, length(position), softness);
}

void main()
{
	const float pace = 5;
	vec2 position = (texCoord*2 - 1);
	vec4 texcolor = texture(u_texture1, texCoord);

	vec3 nrml = normalize(normal);
	vec3 absPos = vertPos;
	vec3 camdir = normalize(u_cameraPos - absPos);
	float b = dot(nrml, camdir) * 0.5 + 0.5;
	float r = -dot(nrml, normalize(cross(camdir, vec3(0.0, 1.0, 0.0)))) * 0.5 + 0.5;
	vec3 right = normalize(cross(u_cameraPos, vec3(0.0, 1.0, 0.0)));
	float g = -dot(nrml, normalize(cross(camdir, right))) * 0.5 + 0.5;

	float timeSin = (sin(u_time) + 1)/2;
	float timeCos = (cos(u_time) + 1)/2;
	float time = (mod(u_time, 2.0 * pace) - pace)/pace;
	float pos1 = position.x + sin((position.y + time)*PI)/2;
	float pos2 = position.y + sin((position.x + time)*PI)/2;
	//FragColor = vec4(vec3((((cos(pos1 * PI)+1.0)*0.5 + (sin(pos2 * PI)+1.0)*0.5)*0.5),
	//					  (((sin(pos1 * PI)+1.0)*0.5 * (cos(pos2 * PI)+1.0)*0.5)),
	//					  (((cos(pos1 * PI)+1.0)*0.5 + (cos(pos2 * PI)+1.0)*0.5)*0.5)), 1.0);
	//FragColor = vec4(vec3(0.5 + (timeSin-0.5)*texCoord.y, timeSin * (texCoord.y*0.85 + 0.15), texCoord.y * timeSin ), 1.0);
	//FragColor = vec4(vec3(min((cos(pos1.x * PI)+1.0)*0.5, (sin(pos2.x * PI)+1.0)*0.5)), 1.0);
	FragColor = vec4(r, g, b, 1.0);
	//FragColor = vec4(absPos, 1.0);
}
