#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "Terrain.hpp"
#include "Region.hpp"

struct Scene {
	Terrain terrain;
	std::vector<Region> regions;

	std::vector<vec3> instances;

	void GenSceneFromTerrain(unsigned int, unsigned int, unsigned int, unsigned int);
};

#endif
