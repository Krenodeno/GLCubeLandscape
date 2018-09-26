#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <image.h>
#include <vec.h>

#include <vector>

struct Terrain {
	Image image;
	Point a, b;

	/** Retourne l'altitude interpollée dans l'espace entre a et b au point (i,j) de l'image
	 * x : position en largeur de l'image, doit etre entre 0 et image.width()
	 * y : position en hauteur de l'image, doit etre entre 0 et image.height()
	 */
	float getHeight(float x, float y) {
		return a.y + (b.y - a.y) * image.sample(x, y).r;
	}

	/** Retourne le point de l'espace 3D entre a et b au point (x,y) de l'image
	 * interpole la hauteur donné par la heightmap
	 * x : position en largeur de l'image, doit etre entre 0 et image.width()
	 * y : position en hauteur de l'image, doit etre entre 0 et image.height()
	 */
	vec3 getPoint(float x, float y) {
		float px = a.x + (b.x - a.x) * (x / (float)(image.width() - 1));
		float pz = a.z + (b.z - a.z) * (y / (float)(image.height() - 1));
		return vec3(px, getHeight(x, y), pz);
	}

	/** Retourne le gradient du pixel (i,j) de l'image
	 * i : position en largeur de l'image
	 */
	vec2 Gradient(int i, int j) {
		float x, y;

		// Cas général
		if (i > 0 && i < image.width()-1)
			x = (getHeight(i+1, j) - getHeight(i-1, j)) / 2;
		if (j > 0 && j < image.height()-1)
			y = (getHeight(i, j+1) - getHeight(i, j-1)) / 2;

		// Cas limites : bords
		if (i == 0)
			x = getHeight(i, j) - getHeight(i+1, j);
		if (i == image.width()-1)
			x = getHeight(i-1, j) - getHeight(i, j);
		if (j == 0)
			y = getHeight(i, j) - getHeight(i, j+1);
		if (j == image.height()-1)
			y = getHeight(i, j-1) - getHeight(i, j);

		return vec2(x, y);
	}

	/** Retourne la pente au pixel (i,j) de l'image
	 * i : position en largeur de l'image
	 */
	float getSlope(int i, int j) {
		auto G = Gradient(i, j);
		return length(Vector(G.x, 0.f, G.y));
	}

	/** Retourne l'ensemble des points de la bounding box en fonction de la heightmap
	 * unit : La taille des voxels (donc l'écart entre deux points)
	 */
	std::vector<vec3> voxelize(float, unsigned int, unsigned int, unsigned int, unsigned int);

	float minNeighborsHeight(float, float);

};

#endif
