#include "Object.h"

Object::Object(Model* model) {
	this->model = model;
	this->matrix = identity_mat4();


}