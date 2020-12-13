#pragma once
// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"
#include "Shaders.h"


struct ModelData
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
};
class Model
{

public:
	unsigned int vao;
	ModelData mesh_data;
	mat4 model_matrix;
	Shaders* shaders;
	vec3 color;
	const char* image_name;
	Model(const char* mesh_name, Shaders* shaders, vec3 color, const char* image_name);
	void generateVAO();

private: 
	ModelData load_mesh(const char* file_name);
};

