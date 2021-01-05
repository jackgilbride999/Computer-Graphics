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

#include "Model.h"

#pragma once
class Object
{
public:
	mat4 matrix;
	Model* model;
	vec3 color;
	Object(Model * model, vec3 color);
	Object();
};

