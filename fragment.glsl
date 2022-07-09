#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 vertColor;
in vec3 normal;
in vec3 vertPos;
in vec3 vertInd;
in mat3 TBN;

uniform sampler2D u_baseColor;
uniform sampler2D u_roughness;
uniform sampler2D u_normal;
uniform sampler2D u_emissive;
uniform samplerCube u_cubemap;
uniform ivec2 u_resolution;
uniform float u_time;
uniform float u_F0;
uniform float u_g_inversion;
uniform int u_F1;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;

const float PI = 3.141592653589793238462;
const float NORMAL_ERROR = 0.0/255;


vec3 basecolor_eval(sampler2D tex, vec2 coord);
vec3 ao_rough_metal_eval(sampler2D tex, vec2 coord);
vec3 emmisive_eval(sampler2D tex, vec2 coord);
vec3 normal_eval(sampler2D tex, vec2 coord);

vec3 diffuse(vec3 normal, vec3 lightdir, vec3 radiance, vec3 ambient, vec3 basecolor);
vec3 diffuse_env(vec3 normal, vec3 basecolor);
vec4 GGX(vec3 normal, vec3 camdir, vec3 lightdir, vec3 radiance, float roughness, vec3 albedo, float metalness);
vec3 reflection(vec3 normal, vec3 camdir, vec3 albedo, float roughness, float metalness);
vec3 fresnel(vec3 facet_dir, vec3 dir, vec3 initial_value, vec3 critical_angle);
vec3 fresnel_schlick(vec3 facet_dir, vec3 dir, vec3 initial_value);
vec3 fresnel_roughness(vec3 facet_dir, vec3 dir, vec3 initial_value, float roughness);
float fast_acos(float x);
float to_bw(vec3 color);

void main()
{
	const float pace = 0.05;
	float timeSin = sin(mod(u_time * pace, 1.0) * PI);
	vec2 position = (texCoord*2 - 1);

	vec3 basecolor = basecolor_eval(u_baseColor, texCoord);
	//basecolor = vec3(1);

	vec3 aoRoughMetal = ao_rough_metal_eval(u_roughness, texCoord);

	float ao = aoRoughMetal.x;
	float roughness = aoRoughMetal.y;
	//roughness = u_F0;
	float metalness = aoRoughMetal.z;
	//metalness = u_F0;
	
	vec3 emissive = emmisive_eval(u_emissive, texCoord);

	vec3 nrml = normal_eval(u_normal, texCoord);

	vec3 pos = vertPos;

	float camdist = length(u_cameraPos - pos);
	
	vec3 camdir = normalize(pos - u_cameraPos);

	vec3 lightdir = normalize(pos - u_lightPos);
	float lightdist = length(u_lightPos - pos);
	vec3 radiance = u_lightColor * 100 / (lightdist * lightdist);

	vec3 hwv = normalize(camdir + lightdir);

	vec3 n = normalize(normal);

	vec4 ggx = GGX(nrml, camdir, lightdir, radiance, roughness, basecolor, metalness);
	vec3 spec = ggx.xyz;
	float f = ggx.w;
	f = to_bw(fresnel_roughness(nrml, camdir, vec3(0.04), roughness));
	vec3 ref = reflection(nrml, camdir, basecolor, roughness, metalness);
	vec3 dirdif = diffuse(nrml, lightdir, radiance, vec3(0.0), basecolor);
	dirdif = (1 - f) * dirdif;
	vec3 envdif = diffuse_env(nrml, basecolor);
	vec3 dif = envdif + dirdif;
	vec3 color = (spec + (1 - metalness) * (dif));
	color += ref;
	//color = vec3(f);
	//color = (vec3(to_bw(color)));
	color = (color/(color + 1)) * ao + emissive;
	FragColor = vec4(color, 1.0);
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

vec3 diffuse(vec3 normal, vec3 lightdir, vec3 radiance, vec3 ambient, vec3 basecolor)
{
	vec3 color;
	float intensity = max(dot(normal, -lightdir), 0);
	vec3 light = intensity * radiance / PI;
	light += ambient;
	color = basecolor * light;
	return color;
}

vec3 diffuse_env(vec3 normal, vec3 basecolor)
{
	vec3 ref = -normal;
	vec3 color = basecolor;
	color *= clamp(textureLod(u_cubemap, ref, 4.5).xyz, 0, 1);
	return color;
}

vec4 GGX(vec3 normal, vec3 camdir, vec3 lightdir, vec3 radiance, float roughness, vec3 albedo, float metalness)
{
	
	vec3 color;
	vec3 cd = 1.0 - albedo;
	cd = (cd*cd*cd) * PI/2.0;
	vec3 v = -camdir;
	vec3 l = -lightdir;
	vec3 h = normalize(v + l);
	vec3 n = normal;
	float a2 = roughness * roughness;
	a2 *= a2;
	float d = max(dot(n, h),0.0);
	float c2 = d * d;
	float nv = max(dot(n, v), 0.0);
	float nl = max(dot(n, l), 0.0);
	float k = (roughness+1.0)*(roughness+1.0)/8.0;
	float D = a2/(PI * (c2 * (a2 - 1.0) + 1.0) * (c2 * (a2 - 1.0) + 1.0));
	//vec3 D3 = reflection(n, camdir,roughness);
	float Gv = nv/(nv*(1.0-k)+k);
	float Gl = nl/(nl*(1.0-k)+k);
	float G = Gl*Gv;
	vec3 Fm = fresnel(h, lightdir, 0.166 + 5*albedo/6, cd);
	vec3 Fd = fresnel_schlick(h, lightdir, vec3(0.04));
	vec3 F = Fm * metalness + Fd * (1 - metalness);
	//vec3 F3 = fresnel(n, v, albedo, cd);
	float denom = (4.0*nl*nv + 0.0001);
	color = F * D * G * radiance * nl * 10 / denom;
	//color = F3 * D3 * G * 1 * 10 / denom;
	return vec4(color, to_bw(F));
}

vec3 reflection(vec3 normal, vec3 camdir, vec3 albedo, float roughness, float metalness)
{
	float r = roughness * 7;
	vec3 ref = reflect(camdir, normal);
	ref *= -1;
	ref = normalize(ref);
	ref = textureLod(u_cubemap, ref, r).xyz;
	vec3 dif = -normal;
	dif = textureLod(u_cubemap, dif, 4.7).xyz;
	vec3 Fd = fresnel_roughness(normal, camdir, vec3(0.04), roughness);
	vec3 cd = 1.0 - albedo;
	cd = (cd*cd*cd) * PI/2.0;
	vec3 Fm = fresnel(normal, camdir, 0.166 + 5*albedo/6, cd);
	vec3 F = Fm * metalness + Fd * (1 - metalness);
	ref = F * ref;
	dif = (1 - metalness) * (1 - F) * albedo * dif;
	vec3 color = ref + dif;
	return color;
}

vec3 fresnel(vec3 facet_dir, vec3 dir, vec3 initial_value, vec3 critical_angle)
{
	vec3 col;
	float d = abs(dot(facet_dir, -dir));
	d = fast_acos(d);
	d = d;
	
	vec3 angle_range = PI/2 - critical_angle;
	vec3 angle_fract = (d - critical_angle)/angle_range;
	vec3 x = angle_fract;
	vec3 y = x * x * x;
	y = y * (vec3(1,1,1) - initial_value);
	col = max(y, 0);
	col = col+initial_value;
	return col;
}

vec3 fresnel_schlick(vec3 facet_dir, vec3 dir, vec3 initial_value)
{
	float dp = 1-abs(dot(dir, facet_dir));
	dp = dp * dp * dp * dp * dp;
	return initial_value + (1 - initial_value)*dp;
}

vec3 fresnel_roughness(vec3 facet_dir, vec3 dir, vec3 initial_value, float roughness)
{
	float dp = 1-abs(dot(dir, facet_dir));
	dp = dp * dp * dp * dp * dp;
	return initial_value + max((1 - roughness) - initial_value, 0)*dp;
}

float fast_acos(float x)
{
    float a=1.43+0.59*x;
	a=(a+(2+2*x)/a)/2;
    float b=1.65-1.41*x;
	b=(b+(2-2*x)/b)/2;
    float c=0.88-0.77*x;
	c=(c+(2-a)/c)/2;
    return (8*(c+(2-a)/c)-(b+(2-2*x)/b))/6;
}

float to_bw(vec3 color)
{
	 return 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
}