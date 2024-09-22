#include "include/meshUtils.h"

void calculateMeshCenter(Vertex* vertices, int numVertices, float& centerX, float& centerY, float& centerZ) {
    centerX = 0.0f;
    centerY = 0.0f;
    centerZ = 0.0f;

    for (int i = 0; i < numVertices; ++i) {
        centerX += vertices[i].x;
        centerY += vertices[i].y;
        centerZ += vertices[i].z;
    }

    if (numVertices > 0) {
        centerX /= numVertices;
        centerY /= numVertices;
        centerZ /= numVertices;
    }
}
