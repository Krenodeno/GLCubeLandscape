#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;

uniform mat4 mvpMatrix;

out vec3 frag_position;

void main( )
{
	frag_position = position;
    gl_Position = mvpMatrix * vec4(frag_position, 1);
}

#endif


#ifdef FRAGMENT_SHADER
in vec3 frag_position;

out vec4 fragment_color;

uniform vec4 color;

void main( )
{
	vec4 green = vec4(0, 0.5, 0, 1);
	vec4 blue = vec4(0, 0, 0.5, 1);
	vec4 grey = vec4(0.57, 0.55, 0.52, 1);
	vec4 white = vec4(0.9, 0.9, 0.9, 1);
	if (frag_position.y < 2) {
		fragment_color = blue;
	}
	else if (frag_position.y < 5) {
		fragment_color = green;
	}
	else if (frag_position.y < 10) {
		fragment_color = grey;
	}
	else if (frag_position.y < 20) {
		fragment_color = white;
	}
	else {
		fragment_color = color;
	}

}

#endif
