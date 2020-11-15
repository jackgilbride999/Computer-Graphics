#include "Camera.h"

Camera::Camera(vec3 position, vec3 direction) {
	this -> position = position;
	this -> direction = direction;
	up = vec3(0.0f, 1.0f, 0.0f);
	right = cross(direction, up);
}

void Camera::move(GLfloat forward, GLfloat horizontal, GLfloat vertical) {
	position += direction * forward + right * horizontal + up * vertical;
}

void Camera::rotate(GLfloat x, GLfloat y, GLfloat z) {
	x = -x;
	y = -y;
	z = -z;
	vec3 tempDirection = direction;
	versor directionVersor = normalise(quat_from_axis_deg(x, up.v[0], up.v[1], up.v[2]) * quat_from_axis_deg(y, right.v[0], right.v[1], right.v[2]) * quat_from_axis_deg(z, direction.v[0], direction.v[1], direction.v[2]));
	vec4 directionVec4 = quat_to_mat4(directionVersor).operator*(vec4(direction.v[0], direction.v[1], direction.v[2], 0.0f));
	direction = vec3(directionVec4.v[0], directionVec4.v[1], directionVec4.v[2]);

	versor upVersor = normalise(quat_from_axis_deg(x, up.v[0], up.v[1], up.v[2]) * quat_from_axis_deg(y, right.v[0], right.v[1], right.v[2]) * quat_from_axis_deg(z, tempDirection.v[0], tempDirection.v[1], tempDirection.v[2]));
	vec4 upVec4 = quat_to_mat4(upVersor).operator*(vec4(up.v[0], up.v[1], up.v[2], 0.0f));
	up = vec3(upVec4.v[0], upVec4.v[1], upVec4.v[2]);

	right = cross(direction, up);
}
