#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "Terrain.hpp"
#include "AABB.hpp"

struct Region {
	// Bounding box pour le test de visibilité
	AABB boundingBox;
	// index de la première instance dans la scène
	int firstInstance;
	// nombre d'instance
	unsigned int size;

};

struct Scene {
	Terrain terrain;
	std::vector<Region> regions;

	std::vector<Point> instances;
	std::vector<Vector> normals;

	void genSceneFromTerrain(Vector, Vector);
	void voxelizeTerrainRegion(float, float, Vector);
	float minNeighborsHeight(float, float);
};

#endif
