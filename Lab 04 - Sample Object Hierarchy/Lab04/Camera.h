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
#include <GL/freeglut.h>

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

