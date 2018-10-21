#include "Scene.hpp"

/**
 * terrainSize: définit la taille du terrain généré
 * regionSize: définit la taille de chaque région du terrain
 */
void Scene::genSceneFromTerrain(Vector terrainSize, Vector regionSize) {
	unsigned int nbRegionX = terrainSize.x / regionSize.x;
	unsigned int nbRegionZ = terrainSize.z / regionSize.z;

	terrain.a = Point(-(terrainSize.x/2.f), 0.f, -(terrainSize.z/2.f));
	terrain.b = Point(terrainSize.x/2.f, terrainSize.y, terrainSize.z/2.f);

	// espacement entre le premier point de chaque région
	float sampleX = 1.f / static_cast<float>(nbRegionX);
	float sampleZ = 1.f / static_cast<float>(nbRegionZ);

	for (unsigned int i = 0; i < nbRegionX; ++i) {
		for (unsigned int j = 0; j < nbRegionZ; ++j) {
			int firstInstance = instances.size();
			unsigned int size = instances.size();
			voxelizeTerrainRegion(sampleX * (float)i, sampleZ * (float)j, regionSize);

			size = instances.size() - size;
			float halfCubeSize = 0.5f;
			Point a(-halfCubeSize + terrain.a.x + regionSize.x * i, -halfCubeSize + terrain.a.y, -halfCubeSize + terrain.a.z + regionSize.z * j);
			Point b(2*halfCubeSize + a.x + regionSize.x, 2*halfCubeSize + a.y + regionSize.y, 2*halfCubeSize + a.z + regionSize.z);
			AABB boundingBox = {a, b};
			regions.push_back({boundingBox, firstInstance, size});

			std::cout << "Bounding Box Région : {" << boundingBox.pMin.x << ", " << boundingBox.pMin.y << ", " << boundingBox.pMin.z << " - ";
			std::cout << boundingBox.pMax.x << ", " << boundingBox.pMax.y << ", " << boundingBox.pMax.z << "}";
			std::cout << "\tfirst = " << firstInstance << ", count = " << size << std::endl;
		}
	}
}

/** Construit un vecteur de coordonnées d'instances dont la première est
 * calculée à partir de u et v
 * u, v: coordonnées image entre 0.0 et 1.0 de la première instance
 * regionSize: taille d'une région
 */
void Scene::voxelizeTerrainRegion(float u, float v, Vector regionSize) {

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
			float minNeighborsH = minNeighborsHeight(pu, pv);
			while (p.y - minNeighborsH >= 1.f) {
				// Ajouter un point en dessous
				p.y -= 1.f;
				points.push_back(p);
				normals.push_back(normal);
				minNeighborsH += 1.f;
			}
		}
	}

	// ajouter à la scène
	instances.insert(instances.end(), points.begin(), points.end());
}

float Scene::minNeighborsHeight(float u, float v) {
	float minNeighborsH = std::numeric_limits<float>::max();
	float sampleX = 1.f / (terrain.b.x - terrain.a.x);
	float sampleZ = 1.f / (terrain.b.z - terrain.a.z);
	for (int neighbor = 0; neighbor < 4; neighbor++) {
		float n = std::numeric_limits<float>::max();
		switch (neighbor) {
		case 0:
			if (u >= (float)terrain.image.width())
				break;
			n = terrain.getHeight(u+sampleX, v);
			break;
		case 1:
			if (v >= (float)terrain.image.height())
				break;
			n = terrain.getHeight(u, v+sampleZ);
			break;
		case 2:
			if (u <= 0.f)
				break;
			n = terrain.getHeight(u-sampleX, v);
			break;
		case 3:
			if (v <= 0.f)
				break;
			n = terrain.getHeight(u, v-sampleZ);
		}
		n = std::floor(n);
		minNeighborsH = std::min(n, minNeighborsH);
	}
	return minNeighborsH;
}
