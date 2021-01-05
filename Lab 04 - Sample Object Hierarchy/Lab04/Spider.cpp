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
		this->eyes[j].color = eye_color;
	}

	this->body_color = body_color;
	this->leg_color = leg_color;
	this->eye_color = eye_color;

	this->update_hierarchy();
}

void Spider::scale_hierarchy(vec3 v) {

}

void Spider::translate_hierarchy(vec3 v) {

}

void Spider::rotate_hierarchy(vec3 v) {

}

void Spider::update_hierarchy() {

	this->body.matrix = translate(identity_mat4(), vec3(0.0, 4, 0.0));

	for (int i = 0; i < 8; i++) {
		this->legs[i].matrix = scale(identity_mat4(), leg_scaling_factor);
		if (i % 2 != 0) {
			this->legs[i].matrix = rotate_y_deg(this->legs[i].matrix, 180);
		}
	}
	this->legs[0].matrix = translate(this->legs[0].matrix, vec3(0.0f, 0.0f, -1.5f));
	this->legs[1].matrix = translate(this->legs[1].matrix, vec3(0.0f, 0.0f, -1.5f));

	this->legs[2].matrix = translate(this->legs[2].matrix, vec3(0.0f, 0.0f, -0.667f));
	this->legs[3].matrix = translate(this->legs[3].matrix, vec3(0.0f, 0.0f, -0.667f));

	this->legs[4].matrix = translate(this->legs[4].matrix, vec3(0.0f, 0.0f, 0.167f));
	this->legs[5].matrix = translate(this->legs[5].matrix, vec3(0.0f, 0.0f, 0.167f));

	this->legs[6].matrix = translate(this->legs[6].matrix, vec3(0.0f, 0.0f, 1.0f));
	this->legs[7].matrix = translate(this->legs[7].matrix, vec3(0.0f, 0.0f, 1.0));



	for (int i = 0; i < 8; i++) {
		this->legs[i].matrix = this->body.matrix * this->legs[i].matrix;
	}

	for (int j = 0; j < 7; j++) {
		this->eyes[j].matrix = scale(identity_mat4(), eye_scaling_factor);
		if (j < 4)
			this->eyes[j].matrix = translate(this->eyes[j].matrix, vec3(j * 0.25 - 0.4, 0.3, 2.9));
		else
			this->eyes[j].matrix = translate(this->eyes[j].matrix, vec3(j * 0.25 - 1.25, 0.45, 2.9));
		this->eyes[j].matrix = this->body.matrix * this->eyes[j].matrix;

	}
}
