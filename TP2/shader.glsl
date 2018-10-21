#version 430

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
//layout (location = 4) in vec4 color;
layout (location = 5) in vec3 instancePos;

out vec2 vertex_texcoord;
out vec3 light_pos;
out vec3 frag_normal;
out vec3 frag_position;
out vec4 lightSpaceFragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpace;

void main()
{
	mat4 mvp = projection * view * model;
	vec3 pos = position + instancePos;

	// Vertex output
	gl_Position = mvp * vec4(pos, 1.0);

	// To fragment shader
	vertex_texcoord = texcoord;
	frag_position = pos;
	lightSpaceFragPos = lightSpace * vec4(pos, 1.0);
	frag_normal = normal;
	light_pos = vec3(0, 1000, 0);
}

#endif
#ifdef FRAGMENT_SHADER

#define M_PI 3.1415926535897932384626433832795

in vec2 vertex_texcoord;
in vec3 light_pos;
in vec3 frag_normal;
in vec3 frag_position;
in vec4 lightSpaceFragPos;

uniform float specular_factor;
uniform vec3 view_pos;

out vec4 fragment_color;

uniform sampler2D texture0;
uniform sampler2D shadowMap;

float diffuse()
{
	return 1.0 / M_PI;
}

float specular(vec3 position, vec3 normal, vec3 eye, vec3 light, float m)
{
	// Blinn-Phong Specular Shading
	vec3 l = normalize(light - position);

	vec3 o = normalize(eye - position);

	vec3 h = (l + o) / 2.0;

	float cos_theta_h = max(0.0, dot(h, normalize(normal)));

	float coeff = (m + 8) / (8 * M_PI);

	return coeff * pow(cos_theta_h, m);
}

float lambert(vec3 normal, vec3 light, vec3 position) {
	return max(0.0, dot(normalize(normal), normalize(light - position)));
}

float shadow(vec4 lightSpacePosition) {
	// Needed in case of a perspective projection
	vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
	// Range is [-1,1], change it to [0,1]
	projCoords = (projCoords * 0.5) + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	// check wether current frag in in shadw or not
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{

	vec3 baseColor;

	vec3 green = vec3(0, 0.5, 0);
	vec3 sand = vec3(0.5, 0.3, 0.1);
	vec3 blue = vec3(0, 0, 0.5);
	vec3 grey = vec3(0.57, 0.55, 0.52);
	vec3 white = vec3(0.9, 0.9, 0.9);
	if (frag_position.y < 1) {
		baseColor = blue;
	}
	else if (frag_position.y < 3) {
		baseColor = sand;
	}
	else if (frag_position.y < 10) {
		baseColor = green;
	}
	else if (frag_position.y < 30) {
		baseColor = grey;
	}
	else if (frag_position.y < 40) {
		baseColor = white;
	}

	float k = 0.5;

	float reflectance = (k * diffuse()) + ((1 - k) * specular(frag_position, frag_normal, view_pos, view_pos, specular_factor));

	float cos_theta = lambert(frag_normal, view_pos, frag_position);

	vec4 texture_color = texture(texture0, vertex_texcoord);

	fragment_color = texture_color * vec4(baseColor * /*(1.0 - shadow(lightSpaceFragPos)) * */reflectance.xxx * cos_theta, 1.0);
	//fragment_color = vec4(abs(frag_normal), 1.0);
	// material_color = base_color * diffuse + light_color * specular
	// fragment_color = shadowfactor * material_color
}

#endif
