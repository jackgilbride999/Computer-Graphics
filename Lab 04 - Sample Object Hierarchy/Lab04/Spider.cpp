#include "Spider.h"

Spider::Spider(Model* body_model, Model* leg_model, Model* eye_model) {
	this->body = Object(body_model);
	this->legs = new Object[8];
	for (int i = 0; i < 8; i++) {
		this->legs[i].model = leg_model;
	}
	this->eyes = new Object[7];
	for (int j = 0; j < 7; j++) {
		this->eyes[j].model = eye_model;
	}
}