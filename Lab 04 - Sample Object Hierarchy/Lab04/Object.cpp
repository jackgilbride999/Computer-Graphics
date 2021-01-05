#include "Object.h"

Object::Object(Model* model, vec3 color) {
	this->model = model;
	this->matrix = identity_mat4();
	this->color = color;
}

Object::Object() {
	this->matrix = identity_mat4();
}