#pragma once
#ifndef MESHUTILS_H
#define MESHUTILS_H

#include <assimp/scene.h>
#include <GL/glew.h>
#include <string>

using namespace std;

struct MyModel {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int indexCount;
    string name;
};

MyModel loadModel(const std::string& path);
MyModel processModel(aiMesh* mesh);

#endif