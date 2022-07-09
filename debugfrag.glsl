#version 330 core

in vec2 texCoord;
in vec3 vertColor;
in vec3 normal;
in vec3 vertPos;
in vec3 vertInd;
in mat3 TBN;

out vec4 FragColor;

uniform float u_time;
uniform int u_mode;
uniform sampler2D u_baseColor;
uniform sampler2D u_roughness;
uniform sampler2D u_emissive;
uniform sampler2D u_normal;
uniform mat3 u_camdirs;
uniform vec3 u_cameraPos;
uniform float u_g_inversion;

const float PI = 3.141592653589793238463;
const float NORMAL_ERROR = 2.0/255;

vec3 basecolor_eval(sampler2D tex, vec2 coord);
vec3 ao_rough_metal_eval(sampler2D tex, vec2 coord);
vec3 emmisive_eval(sampler2D tex, vec2 coord);
vec3 normal_eval(sampler2D tex, vec2 coord);

void main()
{
    vec3 time;
    time.r = mod(u_time, 1);
    time.g = mod(u_time, 10)/10;
    time.b = mod(u_time, 60)/60;

    vec3 basecolor = basecolor_eval(u_baseColor, texCoord);

	vec3 aoRoughMetal = ao_rough_metal_eval(u_roughness, texCoord);

	float ao = aoRoughMetal.x;
	float roughness = aoRoughMetal.y;
	float metalness = aoRoughMetal.z;
	
	vec3 emissive = emmisive_eval(u_emissive, texCoord);


    vec3 vertex_normal = normal;
    vec3 fragment_normal = normalize(vertex_normal);
	vec3 map_normal = normal_eval(u_normal, texCoord);

    vec3 camdir = normalize(u_cameraPos - vertPos);
    vec3 tb = normalize(cross(camdir,u_camdirs[0])+cross(u_camdirs[1],camdir));
    vec3 camright = normalize(cross(tb, camdir) + tb);
	vec3 camup = cross(camdir, camright);
	float bb = dot(camdir, map_normal);
	float rr = dot(camright, map_normal);
	float gg = dot(camup, map_normal);
	vec3 screen_space_normal = vec3(rr, gg, bb) *0.5 + 0.5;

	switch (u_mode) {
    case 1:
        FragColor = vec4(vertInd, 1.0);
        break;
    case 2:
        FragColor = vec4(vertColor, 1.0);
        break;
    case 3:
        FragColor = vec4(map_normal * 0.5 + 0.5, 1.0);
        break;
    case 4:
        FragColor = vec4(basecolor, 1.0);
        break;
    case 5:
        //FragColor = vec4(vertPos/100 * 0.5 + 0.5, 1.0);
        FragColor = vec4(vec3(ao), 1.0);
        break;
    case 6:
        FragColor = vec4(vec3(roughness), 1.0);
        break;
    case 7:
        FragColor = vec4(vec3(metalness), 1.0);
        break;
    case 8:
        FragColor = vec4(texture(u_normal, texCoord).xyz, 1.0);
        break;
    case 9:
        FragColor = vec4(screen_space_normal, 1.0);
        break;
    }
}

vec3 basecolor_eval(sampler2D tex, vec2 coord)
{
	vec3 basecolor = vec3(1,0,1);
	if(textureSize(tex, 0).x > 1)
    {
		basecolor = texture(tex, coord).xyz;
    }
	return basecolor;
}
vec3 ao_rough_metal_eval(sampler2D tex, vec2 coord)
{
	vec3 ao_rough_metal; 
	ao_rough_metal.x = 1;
	ao_rough_metal.y = 0;
	ao_rough_metal.z = 1;
	if(textureSize(tex, 0).x > 1)
    {
		ao_rough_metal.x =	texture(tex, coord).r;
		ao_rough_metal.y =	texture(tex, coord).g;
		ao_rough_metal.z =	texture(tex, coord).b;
		//ao_rough_metal.y = 1 - ao_rough_metal.y;
    }
	return ao_rough_metal;
}
vec3 emmisive_eval(sampler2D tex, vec2 coord)
{
	vec3 emissive = vec3(0);
    if(textureSize(tex, 0).x > 1)
    {
		emissive =	texture(tex, coord).rgb;
    }
	return emissive;
}
vec3 normal_eval(sampler2D tex, vec2 coord)
{
	vec3 normalMap = normalize(normal);
    if(textureSize(tex, 0).x > 1)
    {
		vec3 read = normalize((texture(tex, coord).xyz)*2-1);
		if (u_g_inversion == 1)
		{
			read.g = -read.g;
		}
		normalMap = normalize(TBN * read);
    }
	return normalMap;
}