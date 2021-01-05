#include "Spider.h"

Spider::Spider(Model* body_model, Model* leg_model, Model* eye_model, vec3 body_color, vec3 leg_color, vec3 eye_color) {
	this->body = Object(body_model, body_color);
	this->legs = new Object[8];
	for (int i = 0; i < 8; i++) {
		this->legs[i].model = leg_model;
		this->legs[i].color = leg_color;
	}
	this->eyes = new Object[7];
	for (int j = 0; j < 7; j++) {
		this->eyes[j].model = eye_model;
		this->legs[j].color = eye_color;
	}
}