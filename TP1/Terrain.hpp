#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <image.h>
#include <vec.h>

#include <vector>

struct Terrain {
	Image image;
	Point a, b;

	/** Retourne l'altitude dans l'espace entre a et b au point (i,j) de l'image
	 * i : position en largeur de l'image
	 * j : position en hauteur de l'image
	 */
	float getHeight(int i, int j) {
		return a.y * (b.y - a.y) * image(i, j).r;
	}

	float getHeight(float x, float y) {
		return a.y * (b.y - a.y) * image.sample(x, y).r;
	}

	/** Retourne le point de l'espace entre a et b au pixel (i,j) de l'image
	 */
	Point getPoint(int i, int j) {
		float x = a.x + (b.x - a.x) * ((float)i / (float)(image.width() - 1));
		float z = a.z + (b.z - a.z) * ((float)j / (float)(image.height() - 1));
		return Point(x, getHeight(i, j), z);
	}

	/** Retourne le gradient du pixel (i,j) de l'image
	 */
	vec2 Gradient(int i, int j) {
		float x, y;

		// Cas général
		if (i > 0 && i < image.width()-1)
			x = (getHeight(i+1, j) - getHeight(i-1, j)) / 2;
		if (j > 0 && j < image.height()-1)
			y = (getHeight(i, j+1) - getHeight(i, j-1)) / 2;

		// Cas limites
		if (i == 0)
			x = getHeight(i, j) - getHeight(i+1, j);
		if (i == image.width()-1)
			x = getHeight(i-1, j) - getHeight(i, j);
		if (j == 0)
			x = getHeight(i, j) - getHeight(i, j+1);
		if (j == image.height()-1)
			y = getHeight(i, j-1) - getHeight(i, j);

		return vec2(x, y);
	}

	/** Retourne la pente au pixel (i,j) de l'image
	 */
	float getSlope(int i, int j) {
		auto G = Gradient(i, j);
		return length(Vector(G.x, 0.f, G.y));
	}

	/** Retourne l'ensemble des points de la bounding box en fonction de la heightmap
	 * unit : La taille des voxels (donc l'écart entre deux points)
	 */
	std::vector<vec3> voxelize(float unit) {
		std::vector<vec3> instances;

		float nbCaseX = (b.x - a.x) / unit;
		float nbCaseZ = (b.z - a.z) / unit;
		float nbCaseH = (b.y - a.y) / unit;

		float imageWidth = image.width();
		float imageHeight = image.height();

		float sampleX = imageWidth / nbCaseX;
		float sampleZ = imageHeight / nbCaseZ;

		for (int i = 0; i < nbCaseX; ++i) {
			for (int j = 0; j < nbCaseZ; ++j) {
				// coordonnées monde
				float x = a.x + unit * i;
				float z = a.z + unit * j;
				// coordonnées heightmap
				float u = sampleX * i;
				float v = sampleZ * j;
				auto y = std::floor((b.y - a.y) * image.sample(u, v).r);
				vec3 p(x, y, z);
				instances.push_back(p);
			}
		}
		return instances;
	}

};

#endif
