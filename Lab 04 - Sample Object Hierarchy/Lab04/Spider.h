#pragma once
#include "Object.h"
class Spider
{
public:
	Object body;
	Object* eyes;
	Object* legs;
	Spider(Model * body_model, Model * leg_model, Model * eye_model);
};

