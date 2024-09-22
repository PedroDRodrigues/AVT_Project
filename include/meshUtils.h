#pragma once
#ifndef MESHUTILS_H
#define MESHUTILS_H

struct Vertex {
    float x, y, z;
};

void calculateMeshCenter(Vertex* vertices, int numVertices, float& centerX, float& centerY, float& centerZ);

#endif
