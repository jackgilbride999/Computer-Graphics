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

class Camera
{
public:
	vec3 up;
	vec3 direction;
	vec3 position;
	vec3 right;
	Camera(vec3 position, vec3 direction);
	void move(GLfloat forward, GLfloat horizontal, GLfloat vertical);
	void rotate(GLfloat x, GLfloat y, GLfloat z);

};

