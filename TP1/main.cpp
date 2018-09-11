#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"		// orbiter
#include "draw.h"			// draw()
#include "app_time.h"		// AppTime
#include "program.h"		// program
#include "uniforms.h"		// uniforms
#include "vec.h"


class TP : public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppTime(1024, 640) {}

    // creation des objets de l'application
    int init( )
    {
		// Mesh
		m_objet= read_mesh("data/cube.obj");

		// model matrix for instances
		float padding = 2.f;
		int size = 100;
		int center = size / 2;
		for (int i = -center; i < center; ++i)
			for (int j = -center; j < center; ++j)
				for (int k = -center; k < center; ++k)
					instances.push_back(Translation(padding * i, padding * k, padding * j));

		// Camera
		m_camera.lookat(Point(), 100);

		// Shader program
		program = read_program("TP1/instancing.glsl");
		program_print_errors(program);
		glUseProgram(program);

		// Vertex buffer (location=0)
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, m_objet.vertex_buffer_size(), m_objet.vertex_buffer(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Texture coords buffer (location=1)
		glGenBuffers(1, &texcoords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, m_objet.texcoord_buffer_size(), m_objet.texcoord_buffer(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Texture (location=2)
		m_texture = read_texture(2, "data/debug2x2red.png");

		// instances (location=5, size=4)
		glGenBuffers(1, &instance_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(Transform), instances.data(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		{
			// Vertex (location=0)
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// Texture coords (location=1)
			glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			//instances (location=5, size=4)
			glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
			GLsizei vec4Size = sizeof(vec4);
			GLuint location = 5;
			for (int i = 0; i < 4; ++i) {
				GLuint loc = location + i;
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
				glVertexAttribDivisor(loc, 1);
			}

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
        m_objet.release();
        glDeleteTextures(1, &m_texture);

		glDeleteVertexArrays(0, &vao);
		glDeleteBuffers(1, &vertex_buffer);

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
		program_use_texture(program, "texture0", 0, m_texture);
		glBindVertexArray(vao);
		{
			glDrawArraysInstanced(GL_TRIANGLES, 0, m_objet.vertex_count(), instances.size());
		}
		glBindVertexArray(0);
		glUseProgram(0);

        return 1;
    }

protected:
	GLuint program;
    Mesh m_objet;
	GLuint vertex_buffer;
	GLuint vao;
	GLuint texcoords_buffer;
	GLuint instance_buffer;
	std::vector<Transform> instances;
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
