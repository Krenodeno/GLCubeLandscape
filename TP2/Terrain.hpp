#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <image.h>
#include <vec.h>

#include <cassert>
#include <vector>

struct Terrain {
	Image image;
	Point a, b;

	/* Retourne l'altitude d'un point de l'image
	 * u, v coordonnées image entre 0.0 et 1.0
	 */
	 float getHeight(float u, float v) {
		 assert(u >= 0.f && u <= 1.f && v >= 0.f && v <= 1.f);
		 return a.y + (b.y - a.y) * image.sample(u * image.width(), v * image.height()).r;
	 }

	/** Retourne le point dans l'AABB définie par a et b
	 * u, v coordonnées image entre 0.0 et 1.0
	 *
	 */
	 Point getPoint(float u, float v) {
		float px = a.x + (b.x - a.x) * u;
		float py = getHeight(u, v);
		float pz = a.z + (b.z - a.z) * v;
		return Point(px, py, pz);
	 }

	/** Calcul du gradient en fonction de la hauteur au point (u,v) de l'image
	 * u, v coordonnées image entre 0.0 et 1.0
	 */
	vec2 Gradient(float u, float v, float e) {
		float x, y;

		// Cas général
		if (u > 0.f && u < image.width() - e)
			x = (getHeight(u+e, v) - getHeight(u-e, v)) / 2.f;
		if (v > 0.f && v < image.height() - e)
			y = (getHeight(u, v+e) - getHeight(u, v-e)) / 2.f;

		// Cas limites : bords
		if (u - e <= 0.f)
			x = getHeight(u, v) - getHeight(u+e, v);
		if (u >= image.width() - e)
			x = getHeight(u-e, v) - getHeight(u, v);
		if (v - e <= 0.f)
			y = getHeight(u, v) - getHeight(u, v+e);
		if (v >= image.height() - e)
			y = getHeight(u, v-e) - getHeight(u, v);

		return vec2(x, y);
	}

	/** Retourne la normal au point u, v
	 * u, v coordonnées image entre 0.0 et 1.0
	 */
	Vector getNormal(float u, float v, float e) {
		auto G = Gradient(u, v, e);
		return normalize(Vector(-G.x, -G.y, 1));
	}

	/** Retourne la pente au point u, v
	 * u, v coordonnées image entre 0.0 et 1.0
	 */
	float getSlope(float u, float v, float e) {
		auto G = Gradient(u, v, e);
		return length(Vector(G.x, 0.f, G.y));
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

	/*Vector getNormal(int x, int y) {
		auto G = Gradient(i, j);
		return Vector(-G.x, -G.y, 1);
	}*/

	/** Retourne la pente au pixel (i,j) de l'image
	 * i : position en largeur de l'image
	 */
	float getSlope(int i, int j) {
		auto G = Gradient(i, j);
		return length(Vector(G.x, 0.f, G.y));
	}

};

#endif
