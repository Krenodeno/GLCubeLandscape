#version 430

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 4) in vec4 color;
layout (location = 5) in vec3 instancePos;

out vec2 vertex_texcoord;
out vec3 light_pos;
out vec3 cam_pos;
out vec3 frag_normal;
out vec3 frag_position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	mat4 mvp = projection * view * model;
	vec3 pos = position + instancePos;

	// Vertex output
	gl_Position = mvp * vec4(pos, 1.0);

	// To fragment shader
	vertex_texcoord = texcoord;
	frag_position = instancePos;
	frag_normal = normal;
	light_pos = vec3(0, 50, 0);
}

#endif
#ifdef FRAGMENT_SHADER

#define M_PI 3.1415926535897932384626433832795

in vec2 vertex_texcoord;
in vec3 light_pos;
in vec3 frag_normal;
in vec3 frag_position;

uniform float specular_factor;
uniform vec3 view_pos;

out vec4 fragment_color;

uniform sampler2D texture0;

float diffuse(float k)
{
	return k / M_PI;
}

float specular(vec3 position, vec3 normal, vec3 eye, vec3 light, float m)
{
	// Blinn-Phong Specular Shading
	vec3 l = normalize(light - position);

	vec3 o = normalize(eye - position);

	vec3 h = (o - l) / 2.0;

	float cos_theta = max(0.0, dot(h, normal));

	float coeff = (m + 8) / (8 * M_PI);

	return coeff * pow(cos_theta, m);
}

void main()
{
	float k = 0.1;

	vec4 color = vec4(1.0);

	float reflectance = diffuse(k) + ((1 - k) * specular(frag_position, frag_normal, view_pos, light_pos, specular_factor));

	fragment_color = vec4(reflectance.xxx, 1.0);
}

#endif
