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
	unsigned int texture;
	ModelData mesh_data;

	GLfloat rotate_x;
	GLfloat rotate_y;
	GLfloat rotate_z;
	GLfloat translate_x;
	GLfloat translate_y;
	GLfloat translate_z;
	GLfloat scale_x;
	GLfloat scale_y;
	GLfloat scale_z;

	Model(const char* file_name);
	void generateVAO(GLuint shaderProgramID, const char* texture_name);

private: 
	ModelData load_mesh(const char* file_name);
};

