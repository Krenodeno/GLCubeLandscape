#include "Scene.hpp"

void Scene::GenSceneFromTerrain(unsigned int regionSize, unsigned int terrainSizeX, unsigned int terrainSizeY, unsigned int terrainSizeZ) {
	unsigned int nbRegion = (terrainSizeX  / regionSize) * (terrainSizeZ / regionSize);

	terrain.a = Point(-(terrainSizeX/2.f), 0.f, -(terrainSizeX/2.f));
	terrain.b = Point(terrainSizeZ/2.f, terrainSizeY, terrainSizeZ/2.f);

	std::cout << "Nombre de régions = " << nbRegion << "\n";
	std::cout << "Largeur du terrain = " << terrainSizeX << "\n";
	std::cout << "Hauteur du terrain = " << terrainSizeZ << "\n";

	unsigned int sizeX = terrainSizeX / regionSize;
	unsigned int sizeZ = terrainSizeZ / regionSize;

	regions.resize(nbRegion);
	// Pour chaque région, on génère les positions des cubes, on sauvegarde
	// le premier et le nombre de cubes, et on ajoute au tableau d'instances
	for (unsigned int i = 0; i < nbRegion; i++) {
		unsigned int posX = (i * regionSize) % terrainSizeX;
		unsigned int posZ = ((i * sizeZ) / nbRegion) * regionSize;
		auto tmp = terrain.voxelize(1.f, posX, posZ, regionSize, regionSize);
		regions[i].firstInstance = instances.size();
		regions[i].size = tmp.size();
		instances.insert(instances.end(), tmp.begin(), tmp.end());
	}

	std::cout << "Nombre d'instances = " << instances.size() << "\n";
}
