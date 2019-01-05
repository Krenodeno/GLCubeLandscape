# TP Synthèse d'image
## Build
Editer `premake5.lua` pour indiquer le chemin vers gkit2light.
Exécuter le script à la racine du projet suivant pour générer les fichiers de build et compiler :
```shell
premake5 gmake2 && make -C build/
```
Exécuter le programme :
```shell
./bin/TP2
```

Appuyer sur `g` pour faire tourner le soleil.

## TODO-liste
- [x] Générer positions d'instances de cubes à partir de la Heightmap
  - [x] Segmentation par régions
    - [x] Test visibilité
      - [ ] Gestion de la mémoire (FIFO VBO)
- [x] Blinn-Phong
- [x] Textures en fonction de l'altitude/la pente
- [x] Shdow Mapping
