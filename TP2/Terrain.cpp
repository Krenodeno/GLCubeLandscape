#include "Terrain.hpp"

#include "HeightMap.hpp"
/*
CB Terrain::GetCB(const HeightMap &) {
	std::vector<Point> points;

	// On part du principe que les voxels sont de taille unitaire (1.0f)
	// sinon il faudrait diviser par la taille des voxels
	float nbCaseX = terrain.b.x - terrain.a.x;
	float nbCaseZ = terrain.b.z - terrain.a.z;

	// espacement entre chaque point
	float sampleX = 1.f / nbCaseX;
	float sampleZ = 1.f / nbCaseZ;

	for (unsigned int i = 0; i < regionSize.x; ++i) {
		for (unsigned int j = 0; j < regionSize.z; ++j) {
			float pu = u + (sampleX * (float)i);
			float pv = v + (sampleZ * (float)j);
			// coordonnées heightmap (entre 0.0 et 1.0)
			auto p = terrain.getPoint(pu, pv);
			p.y = std::floor(p.y);
			auto normal = terrain.getNormal(pu, pv, sampleX);
			points.push_back(p);
			normals.push_back(normal);
			// Combler les trous
			float minNeighborsH = minNeighborsHeight(u, v);
			while (p.y - minNeighborsH > 1.f) {
				// Ajouter un point en dessous
				p.y -= 1.f;
				points.push_back(p);
				normals.push_back(normal);
				minNeighborsH += 1.f;
			}
		}
	}

}



Point Terrain::getPoint(float u, float v) {

}

vec2 Terrain::Gradient(float u, float v, float e) {

}

Vector Terrain::getNormal(float u, float v, float e) {

}

float Terrain::getSlope(float u, float v, float e) {

}
*/
