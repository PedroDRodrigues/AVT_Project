#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

bool Import3DFromFile(const std::string& pFile, Assimp::Importer& importer, const aiScene*& sc, float& scaleFactor);
std::vector<struct MyMesh> createMeshFromAssimp(const aiScene*& sc, GLuint*& textureIds);