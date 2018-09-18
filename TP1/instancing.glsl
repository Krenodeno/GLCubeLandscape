#version 430

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 4) in vec4 color;
layout (location = 5) in mat4 instanceMatrix;	// takes 4 times a vec4

out vec2 vertex_texcoord;
out vec3 frag_position;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * transpose(instanceMatrix) * vec4(position, 1.0);
	//gl_Position = projection * view * mat4(1.0) * vec4(position, 1.0);
	vertex_texcoord = texcoord;
	vec4 pos = transpose(instanceMatrix) * vec4(position, 1.0);
	frag_position = vec3(pos.xyz);
}

#endif
#ifdef FRAGMENT_SHADER

in vec2 vertex_texcoord;
in vec3 frag_position;

out vec4 fragment_color;

uniform sampler2D texture0;

void main()
{
	vec4 green = vec4(0, 0.5, 0, 1);
	vec4 sand = vec4(0.5, 0.3, 0.1, 1);
	vec4 blue = vec4(0, 0, 0.5, 1);
	vec4 grey = vec4(0.57, 0.55, 0.52, 1);
	vec4 white = vec4(0.9, 0.9, 0.9, 1);
	if (frag_position.y < 1) {
		fragment_color = blue;
	}
	else if (frag_position.y < 3) {
		fragment_color = sand;
	}
	else if (frag_position.y < 10) {
		fragment_color = green;
	}
	else if (frag_position.y < 15) {
		fragment_color = grey;
	}
	else if (frag_position.y < 30) {
		fragment_color = white;
	}
}

#endif
