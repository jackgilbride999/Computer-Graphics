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
};

