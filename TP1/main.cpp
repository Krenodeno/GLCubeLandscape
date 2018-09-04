#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"		// orbiter
#include "draw.h"			// draw()
#include "app_time.h"		// AppTime


class TP : public AppTime
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppTime(1024, 640) {}

    // creation des objets de l'application
    int init( )
    {
        m_objet= read_mesh("data/cube.obj");

		float padding = 2.f;
		int size = 1000;
		int center = size / 2;
		for (int i = -center; i < center; ++i)
			for (int j = -center; j < center; ++j)
				instances.push_back(Translation(padding * i, 0, padding * j));

        m_camera.lookat(Point(), ((size+padding)*size)/2);

        m_texture= read_texture(0, "data/debug2x2red.png");

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

		for(auto m_model : instances)
			draw(m_objet, m_model, m_camera, m_texture);

        return 1;
    }

protected:
    Mesh m_objet;
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
