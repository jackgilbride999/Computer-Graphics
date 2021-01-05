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
	void rotate_hierarchy(vec3 v);

private:
	void update_hierarchy();
	vec3 leg_scaling_factor = vec3(0.6, 0.6, 0.6);
	vec3 eye_scaling_factor = vec3(0.15, 0.15, 0.15);

};

