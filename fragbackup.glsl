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
uniform samplerCube u_cubemap;
uniform ivec2 u_resolution;
uniform float u_time;
uniform float u_F0;
uniform int u_F1;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;

const float PI = 3.141592653589793238462;
const float NORMAL_ERROR = 0.0/255;

vec3 diffuse(vec3 normal, vec3 lightdir, vec3 radiance, vec3 basecolor);
vec3 GGX(vec3 normal, vec3 camdir, vec3 lightdir, vec3 radiance, float roughness, vec3 albedo);
vec3 reflection(vec3 normal, vec3 camdir, float roughness);
vec3 fresnel(vec3 facet_dir, vec3 dir, vec3 initial_value, vec3 critical_angle);
vec3 fresnel_schlick(vec3 facet_dir, vec3 dir, vec3 initial_value);
float fast_acos(float x);

void main()
{
	//const float pace = 0.05;
	//float timeSin = sin(mod(u_time * pace, 1.0) * PI);
	vec2 position = (texCoord*2 - 1);

	vec3 basecolor = vec3(1,0,1);
	if(textureSize(u_baseColor, 0).x > 1)
    {
		basecolor = texture(u_baseColor, texCoord).xyz;
    }

	float roughness = 0;
	if(textureSize(u_roughness, 0).x > 1)
    {
		roughness =	texture(u_roughness, texCoord).x;
		//roughness = 1 - roughness;
    }

	vec3 normalMap = normalize(normal);
    if(textureSize(u_normal, 0).x > 1)
    {
		vec3 read = normalize((texture(u_normal, texCoord).xyz)*2-1);
		normalMap = normalize(TBN * read);
		//normalMap.g = 1 - normalMap.g;
    }

	vec3 nrml = normalMap;


	vec3 pos = vertPos;

	float camdist = length(u_cameraPos - pos);
	
	vec3 camdir = normalize(pos - u_cameraPos);

	vec3 lightdir = normalize(pos - u_lightPos);
	float lightdist = length(u_lightPos - pos);
	vec3 radiance = u_lightColor * 10 / (lightdist * lightdist);

	vec3 hwv = normalize(camdir + lightdir);
	vec3 spec = GGX(nrml, camdir, lightdir, radiance, u_F0, basecolor);
	vec3 ref = reflection(nrml, camdir, pow(roughness, 1.0/3.0));
	vec3 dif = diffuse(nrml, lightdir, radiance, basecolor);
	//vec3 fr = fresnel(nrml, lightdir, basecolor, (PI/2)*(pow(1-basecolor, vec3(1.2,.8,1.1))));
	//vec3 fr = fresnel_schlick(nrml, lightdir, basecolor);
	vec3 color = clamp((spec) * 1, vec3(0), vec3(1));
	FragColor = vec4(color, 1.0);
}

vec3 diffuse(vec3 normal, vec3 lightdir, vec3 radiance, vec3 basecolor)
{
	vec3 color;
	float intensity = max(dot(normal, -lightdir), 0);
	color = basecolor * intensity * radiance;
	return color;
}

vec3 GGX(vec3 normal, vec3 camdir, vec3 lightdir, vec3 radiance, float roughness, vec3 albedo)
{
	
	vec3 color;
	vec3 cd = 1 - albedo;
	cd = cd * cd * cd * PI/2;
	vec3 v = -camdir;
	vec3 l = -lightdir;
	vec3 h = normalize(v + l);
	vec3 n = normal;
	float a2 = roughness * roughness;
	float d = dot(n, h);
	float c2 = d * d;
	float xd = float(d>0);
	float xgv = float((dot(v,h)*dot(v,n))>0);
	float xgl = float((dot(l,h)*dot(l,n))>0);
	float nv = dot(n, v);
	float nl = dot(n, l);
	float k = (roughness+1)*(roughness+1)/8;
	float D = xd * a2/(PI * (c2 * (a2 - 1) + 1) * (c2 * (a2 - 1) + 1));
	float Gv = xgv * nv/(nv*(1-k)+k);
	float Gl = xgl * nl/(nl*(1-k)+k);
	float G = Gl*Gv;
	vec3 F = fresnel(h, lightdir, albedo, cd);
	float denom = 1/(4*nl*nv);
	color = F*D*G*radiance*denom;
	//color = vec3(F);
	return color;
}

vec3 reflection(vec3 normal, vec3 camdir, float roughness)
{
	vec3 ref = reflect(camdir, normal);
	//vec3 ref = normal;
	ref.y = -ref.y;
	ref = normalize(ref);
	vec3 color;
	float r = roughness * 7;
	float l0 = floor(r);
	vec3 t0 = textureLod(u_cubemap, ref, l0).xyz;
	float l1 = ceil(r);
	vec3 t1 = textureLod(u_cubemap, ref, l1).xyz;
	float a = fract(r);
	color = ((t0  / 7.5) * (1-a) + (t1 /7.5) * a);
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