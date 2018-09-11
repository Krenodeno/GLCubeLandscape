#version 430

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 4) in vec4 color;
layout (location = 5) in mat4 instanceMatrix;	// takes 4 times a vec4

out vec2 vertex_texcoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * transpose(instanceMatrix) * vec4(position, 1.0);
	//gl_Position = projection * view * mat4(1.0) * vec4(position, 1.0);
	vertex_texcoord = texcoord;
}

#endif
#ifdef FRAGMENT_SHADER

in vec2 vertex_texcoord;

out vec4 fragment_color;

uniform sampler2D texture0;

void main()
{
	fragment_color = texture(texture0, vertex_texcoord);
}

#endif
