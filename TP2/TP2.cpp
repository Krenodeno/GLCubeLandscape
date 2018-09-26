#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"		// orbiter
#include "draw.h"			// draw()
#include "app_time.h"		// AppTime
#include "program.h"		// program
#include "uniforms.h"		// uniforms
#include "vec.h"			// vec
#include "mat.h"			// mat
#include "image_io.h"		// Image

#include "Terrain.hpp"		// Terrain
#include "Region.hpp"
#include "Scene.hpp"


class TP : public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppTime(1024, 640) {}

    // creation des objets de l'application
    int init( )
    {
		// Mesh
		cube = read_mesh("data/cube.obj");

		scene.terrain.image = read_image("data/Clipboard02.png");

		scene.GenSceneFromTerrain(64, 512, 40, 512);

		// Camera
		m_camera.lookat(Point(), 512);

		// Shader program
		program = read_program("TP2/shader.glsl");
		program_print_errors(program);
		glUseProgram(program);

		// Vertex buffer (location=0)
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, cube.vertex_buffer_size(), cube.vertex_buffer(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Texture coords buffer (location=1)
		glGenBuffers(1, &texcoords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, cube.texcoord_buffer_size(), cube.texcoord_buffer(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Texture (Uniform)
		m_texture = read_texture(2, "data/debug2x2red.png");

		// instances (location=5)
		glGenBuffers(1, &instance_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, scene.instances.size() * sizeof(vec3), scene.instances.data(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		{
			GLuint location = 0;
			// Vertex (location=0)
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(location);

			// Texture coords (location=1)
			location = 1;
			glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
			glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(location);

			// Instances (location=5)
			location = 5;
			glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
			glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
			glEnableVertexAttribArray(location);
			glVertexAttribDivisor(location, 1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		// unbind
		glBindVertexArray(0);

		//unbind program
		glUseProgram(0);

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        return 0;   // ras, pas d'erreur
    }

    // destruction des objets de l'application
    int quit( )
    {
        cube.release();
        glDeleteTextures(1, &m_texture);


		glDeleteVertexArrays(0, &vao);
		glDeleteBuffers(1, &vertex_buffer);
		glDeleteBuffers(1, &texcoords_buffer);
		glDeleteBuffers(1, &instance_buffer);

        return 0;
    }

    // dessiner une nouvelle image
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
        if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
            m_camera.move(mx);
        else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
            m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

		// instanced draw
		glUseProgram(program);
		// uniforms
		program_uniform(program, "projection", m_camera.projection(1024, 640, 50));
		program_uniform(program, "view", m_camera.view());
		program_uniform(program, "model", Transform());
		program_use_texture(program, "texture0", 0, m_texture);
		// bind VAO and draw
		glBindVertexArray(vao);
		{
			glDrawArraysInstanced(GL_TRIANGLES, 0, cube.vertex_count(), scene.instances.size());
		}
		glBindVertexArray(0);
		glUseProgram(0);

        return 1;
    }

protected:

	Scene scene;

	GLuint program;
	Mesh cube;
	// Mesh billboard;
	GLuint vertex_buffer;
	GLuint texcoords_buffer;
	GLuint instance_buffer;
	GLuint vao;
	GLuint m_texture;
	Orbiter m_camera;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer
    TP tp;
    tp.run();

    return 0;
}
