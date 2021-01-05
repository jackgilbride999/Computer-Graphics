#pragma once
#include "Object.h"
class Spider
{
public:
	Object body;
	Object* eyes;
	Object* legs;

	vec3 body_color;
	vec3 leg_color;
	vec3 eye_color;

	Spider(Model* body_model, Model* leg_model, Model* eye_model, vec3 body_color, vec3 leg_color, vec3 eye_color);
	void scale_hierarchy(vec3 v);
	void translate_hierarchy(vec3 v);
	void rotate_hierarchy_x_deg(float deg);
	void rotate_hierarchy_y_deg(float deg);
	void rotate_hierarchy_z_deg(float deg);
	void update_leg_rotation(float delta);


private:
	void update_hierarchy();
	void update_leg_set(GLfloat& leg_rotation, boolean& increasing, float delta);
	vec3 leg_scaling_factor = vec3(0.6, 0.6, 0.6);
	vec3 eye_scaling_factor = vec3(0.15, 0.15, 0.15);
	GLfloat leg_set_1_rotate_x = 0.0f;
	boolean leg_set_1_rotate_x_increasing = true;
	GLfloat leg_set_2_rotate_x = 0.0f;
	boolean leg_set_2_rotate_x_increasing = false;

};

