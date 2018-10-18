#include "AABB.hpp"

bool visible(const Transform& mvp, const AABB& bb) {
	if (visibleInProjected)
		return visibleInWorld(mvp.inverse(), bb);
}

bool visibleInProjected(const Transform& mvp, const AABB& bb) {
	int planes[] = {0, 0, 0, 0, 0, 0};

	// Enumérer les 8 sommets de la boite englobante
	for (unsigned int i = 0; i < 8; i++) {
		Point p = bb.pMin;
		if (i & 1) p.x = bb.pMax.x;
		if (i & 2) p.y = bb.pMax.y;
		if (i & 4) p.z = bb.pMax.z;

		// Transformation du point homogène (x, y, z, w=1) dans le repère projectif
		vec4 h = mvp(vec4(p));

		// Tester la position du point homogène par rapport aux 6 faces de la région
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

bool visibleInWorld(const Transform& mvpi, const AABB& bb) {
	int planes[] = {0, 0, 0, 0, 0, 0};

	// le frustum dans le repère projectif
	AABB frustum = {Point(-1.f, -1.f, -1.f), Point(1.f, 1.f ,1.f)};

	// Enumérer les 8 sommets du frustum
	for (unsigned int i = 0; i < 8; i++) {
		Point h = frustum.pMin;
		if (i & 1) h.x = frustum.pMax.x;
		if (i & 2) h.y = frustum.pMax.y;
		if (i & 4) h.z = frustum.pMax.z;

		// Transformation dans repère monde
		vec4 p = mvpi(vec4(h));

		// Tester la position
		if (p.x < p.w * bb.pMin.x) planes[0]++;	// Trop à gauche
		if (p.x > p.w * bb.pMax.x) planes[1]++;	// Trop à droite

		if (p.y < p.w * bb.pMin.y) planes[2]++;	// Trop bas
		if (p.y > p.w * bb.pMax.y) planes[3]++;	// Trop haut

		if (p.z < p.w * bb.pMin.z) planes[4]++;	// Trop près
		if (p.z > p.w * bb.pMax.z) planes[5]++;	// Trop loin
	}

	// Vérifie si tous les sommets dont du 'mauvais' côté d'une seule face
	for (unsigned int i = 0; i < 6; i++){
		if (planes[i] == 8) {
			return false;	// La boite englobente n'est pas visible
		}
	}
	return true;
}
