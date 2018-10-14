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

//#include "HeighMap.hpp"		// HeightMap
#include "Terrain.hpp"		// Terrain
#include "Scene.hpp"		// Scene

//#include "AABB.hpp"


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

		cpt = 0;
/*
		{
			Point pMin(-256.f, 0.f, -256.f);
			Point pMax(256.f, 40.f, 256.f);
			HeightMap heightmap(read_image("data/Clipboard02.png"));

			Terrain terrain;
			terrain.GetCB(heightmap);


		}
*/
		scene.terrain.image = read_image("data/Clipboard02.png");

		scene.genSceneFromTerrain(Vector(256.f, 40.f, 256.f), Vector(64.f, 64.f, 64.f));

		std::cout << "Nombre d'instances = " << scene.instances.size() << std::endl;

		// Camera
		m_camera.lookat(Point(), 512.f);

		// Shader program
		program = read_program("TP2/shader.glsl");
		program_print_errors(program);
		glUseProgram(program);

		specFactor = 16.f;

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

		// Normal buffer (location=2)
		glGenBuffers(1, &normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		{
			glBufferData(GL_ARRAY_BUFFER, scene.normals.size() * sizeof(vec3), scene.normals.data(), GL_STATIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, cube.normal_buffer_size(), cube.normal_buffer(), GL_STATIC_DRAW);
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

			// Normals (location=2)
			location = 2;
			glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
			glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
			glEnableVertexAttribArray(location);
			glVertexAttribDivisor(location, 1);

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


		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vertex_buffer);
		glDeleteBuffers(1, &normal_buffer);
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
		unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);

		if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
			m_camera.rotation(mx, my);
		else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
			m_camera.move(mx);
		else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
			m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

		if (key_state('r')) {
			clear_key_state('r');
			cpt = (cpt+1) % scene.instances.size();
		}

		Transform projection = m_camera.projection(1024, 640, 50);
		Transform view = m_camera.view();
		Transform model = Transform();
/*
		// TODO TEST DE VISIBILITE
		std::vector<int> firsts;	// indices des instances à dessiner
		std::vector<int> counts;	// nombres d'instances à dessiner
		Transform mvp = projection * view * model;
		Transform mvpi = mvp.inverse();
		for (auto region : scene.regions) {
			if (visible(mvp, region.boundingBox) && visible(mvpi, region.boundingBox)) {
				firsts.push_back(region.firstInstance);
				counts.push_back(region.size);
			}
		}
*/
		//printf(m_console, 0, 3, "Nombre d'instances affichées : %u", firsts.size());
		//std::cout << "first" << scene.region[cpt].firstInstance << "\n";

		// instanced draw
		glUseProgram(program);
		// uniforms
		program_uniform(program, "projection", projection);
		program_uniform(program, "view", view);
		program_uniform(program, "model", model);
		program_uniform(program, "view_pos", m_camera.position());
		program_uniform(program, "specular_factor", specFactor);
		program_use_texture(program, "texture0", 0, m_texture);
		// bind VAO and draw
		glBindVertexArray(vao);
		{
			Transform mvp = projection * view * model;
			//Transform mvpi = mvp.inverse();
			for (const auto& region : scene.regions) {
				if (visible(mvp, region.boundingBox) /*&& visible(mvpi, region.boundingBox)*/) {
					glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, cube.vertex_count(), (GLsizei)region.size, region.firstInstance);
				}
			}
			//glDrawArraysInstanced(GL_TRIANGLES, 0, cube.vertex_count(), scene.instances.size());
		}
		glBindVertexArray(0);
		glUseProgram(0);

		return 1;
	}

protected:
	int cpt;

	Scene scene;

	GLuint program;
	float specFactor;

	Mesh cube;
	// Mesh billboard;
	GLuint vertex_buffer;
	GLuint texcoords_buffer;
	GLuint normal_buffer;
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
