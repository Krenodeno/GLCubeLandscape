#version 430

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 5) in vec3 instancePos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	mat4 mvp = projection * view * model;
	vec3 pos = position + instancePos;

	// Vertex output
	gl_Position = mvp * vec4(pos, 1.0);
}

#endif
#ifdef FRAGMENT_SHADER

void main() {

}

#endif
