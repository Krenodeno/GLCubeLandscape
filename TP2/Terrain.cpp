#include "Terrain.hpp"

std::vector<vec3> Terrain::voxelize(float unit, unsigned int posX, unsigned int posY, unsigned int sizeX, unsigned int sizeY) {
	std::vector<vec3> points;

	float nbCaseX = (b.x - a.x) / unit;
	float nbCaseZ = (b.z - a.z) / unit;
	float nbCaseH = (b.y - a.y) / unit;

	float imageWidth = image.width();
	float imageHeight = image.height();

	float sampleX = imageWidth / nbCaseX;
	float sampleZ = imageHeight / nbCaseZ;

	for (unsigned int i = posX; i < posX + sizeX; ++i) {
		for (unsigned int j = posY; j < posY + sizeY; ++j) {
			// coordonnées heightmap
			float u = sampleX * i;
			float v = sampleZ * j;
			vec3 p = getPoint(u, v);
			p.y = std::floor(p.y);
			points.push_back(p);
			// Combler les trous
			float minNeighborsH = minNeighborsHeight(u, v);
			if (p.y - minNeighborsH > 1.0) {
				// Ajouter un point en dessous TODO en ajouter plusieurs
				vec3 q = p;
				q.y -= 1.0f;
				points.push_back(q);
			}
		}
	}
	return points;
}

float Terrain::minNeighborsHeight(float x, float y) {
	float minNeighborsH = std::numeric_limits<float>::max();
	for (int neighbor = 0; neighbor < 4; neighbor++) {
		vec3 n;
		switch (neighbor) {
		case 0:
			if (x > (float)image.width())
				break;
			n = getPoint(x+1, y);
			break;
		case 1:
			if (y > (float)image.height())
				break;
			n = getPoint(x, y+1);
			break;
		case 2:
			if (x < 1.f)
				break;
			n = getPoint(x-1, y);
			break;
		case 3:
			if (y < 1.f)
				break;
			n = getPoint(x, y-1);
		}
		n.y = std::floor(n.y);
		minNeighborsH = std::min(n.y, minNeighborsH);
	}
	return minNeighborsH;
}
