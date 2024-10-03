#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include "include/geometry.h"
#include <vector>

MyMesh createTerrainMesh(float terrainSize);
MyMesh createWaterMesh(float waterSize);
vector<MyMesh> createHouseMeshes(int numberOfHouses, float terrainSize, float waterSize);
vector<MyMesh> createObstacleMeshes(int numberOfObstacles, float terrainSize, float waterSize);

#endif
