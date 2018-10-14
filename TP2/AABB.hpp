#ifndef AXIS_ALIGNED_BOUNDING_BOX_HPP
#define AXIS_ALIGNED_BOUNDING_BOX_HPP

#include "mat.h"
#include "vec.h"

struct AABB {
	Point pMin, pMax;
};

inline bool visible(const Transform& mvp, const AABB& bb) {
	int planes[] = {0, 0, 0, 0, 0, 0};

	// Enumère les 8 sommets de la boite englobante
	for(unsigned int i = 0; i < 8; i++) {
		Point p = bb.pMin;
		if (i & 1) p.x = bb.pMax.x;
		if (i & 2) p.y = bb.pMax.y;
		if (i & 4) p.z = bb.pMax.z;

		// Trnasformation du point homogène (x, y, z, w=1)
		vec4 h = mvp(vec4(p));

		// Teste la position du point homogène par rapport aux 6 faces de la région
		if (h.x < -h.w) planes[0]++;	// Trop à gauche
		if (h.x > h.w)  planes[1]++;	// Trop à droite

		if (h.y < -h.w) planes[2]++;	// Trop bas
		if (h.y > h.w)  planes[3]++;	// Trop haut

		if (h.z < -h.w) planes[4]++;	// Trop près
		if (h.z > h.w)  planes[5]++;	// Trop loin
	}

	// Vérifie si tous les sommets dont du 'mauvais' côté d'une seule face
	for (unsigned int i = 0; i < 6; i++)
		if (planes[i] == 8)
			return false;	// La boite englobente n'est pas visible

	// L'objet doit être visible, ou pas, il faut aussi  le tester dans l'autre sens
	// ie: rappeler la fonction avec la matrice mvp inverse
	return true;
}

#endif
