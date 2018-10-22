#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"		// orbiter
#include "draw.h"			// draw()
#include "app_time.h"		// AppTime
#include "program.h"		// program
#include "uniforms.h"		// uniforms
#include "vec.h"			// vec
#include "mat.h"			// mat, Transform
#include "image_io.h"		// Image

//#include "HeighMap.hpp"		// HeightMap
#include "Terrain.hpp"		// Terrain
#include "Scene.hpp"		// Scene

//#include "AABB.hpp"		// AABB

Transform ortho(float left, float right, float top, float bottom, float near, float far) {
	return Transform(
		2.f/(right-left), 0, 0, -((right+left)/(right-left)),
		0, 2.f/(top-bottom), 0, -((top+bottom)/(top-bottom)),
		0, 0, -(2.f/(far-near)), -((far+near)/(far-near)));
}


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

		// Texture (Uniform)
		m_texture = read_texture(0, "data/grass_top.png");

		// Camera
		m_camera.lookat(Point(), 512.f);

		// Sun
		sun.lookat(Point(-128.f, 0.f, 128.f), Point(-128.f, 40.f, 128.f));

		depthShader = read_program("TP2/depth.glsl");
		program_print_errors(depthShader);

		// Shader program
		program = read_program("TP2/shader.glsl");
		program_print_errors(program);
		//glUseProgram(program);

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
			//glBufferData(GL_ARRAY_BUFFER, scene.normals.size() * sizeof(vec3), scene.normals.data(), GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, cube.normal_buffer_size(), cube.normal_buffer(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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
			glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(location);
			//glVertexAttribDivisor(location, 1);

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

		// Depth pass
		glGenFramebuffers(1, &depthpass);
		glBindFramebuffer(GL_FRAMEBUFFER, depthpass);

		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

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

		// Depth pass
		glDeleteFramebuffers(1, &depthpass);
		glDeleteTextures(1, &depthTexture);

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
		// deplace la camera
		int mx, my;
		unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);

		if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
			m_camera.rotation(mx, my);
		else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
			m_camera.move(mx);
		else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
			m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());


		// Depth Pass
		glViewport(0, 0, 1024, 1024);	// depth texture size
		glBindFramebuffer(GL_FRAMEBUFFER, depthpass);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// matrix
		Transform sunProj = ortho(-128, 128, -128, 128, -128, 128);
		Transform sunView = sun.view();
		Transform sunModel = Transform();
		Transform sunMVP = sunProj * sunView * sunModel;
		// draw
		glUseProgram(depthShader);
		program_uniform(depthShader, "projection", sunProj);
		program_uniform(depthShader, "view", sunView);
		program_uniform(depthShader, "model", sunModel);

		glBindVertexArray(vao);
		for (const auto& region : scene.regions) {
			if (visible(sunMVP, region.boundingBox)) {
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, cube.vertex_count(), (GLsizei)region.size, region.firstInstance);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Color Pass
		glViewport(0, 0, window_width(), window_height());	// Screen size
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Transform projection = m_camera.projection(window_width(), window_height(), 50);
		Transform view = m_camera.view();
		Transform model = Transform();
		Transform mvp = projection * view * model;

		// instanced draw
		glUseProgram(program);
		// uniforms
		program_uniform(program, "projection", projection);
		program_uniform(program, "view", view);
		program_uniform(program, "model", model);
		program_uniform(program, "lightSpace", sunMVP);
		program_uniform(program, "viewWorldPos", m_camera.position());
		program_uniform(program, "lightWorldPos", sun.position());
		program_use_texture(program, "texture0", 0, m_texture);
		program_use_texture(program, "shadowMap", 0, depthTexture);

		// glBindVertexArray(vao);

		// Test de visibilité
		for (const auto& region : scene.regions) {
			if (visible(mvp, region.boundingBox)) {
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, cube.vertex_count(), (GLsizei)region.size, region.firstInstance);
			}
		}

		glBindVertexArray(0);
		glUseProgram(0);

		return 1;
	}

protected:
	// Scene
	Scene scene;
	Mesh cube;
	// Mesh billboard;
	GLuint vertex_buffer;
	GLuint texcoords_buffer;
	GLuint normal_buffer;
	GLuint instance_buffer;
	GLuint vao;

	// Color pass
	GLuint m_texture;
	Orbiter m_camera;
	GLuint program;

	// Depth pass
	GLuint depthShader;
	GLuint depthpass;
	GLuint depthTexture;
	Orbiter sun;
};


int main( int argc, char **argv )
{
	// il ne reste plus qu'a creer un objet application et la lancer
	TP tp;
	tp.run();

	return 0;
}
