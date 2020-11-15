// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"

#include "Object.h"

#include "Shaders.h"

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define SPIDER_MESH_NAME "spider.dae"
#define LEG_MESH_NAME "leg.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
using namespace std;
GLuint shaderProgramID;

int width = 800;
int height = 600;

Object spider = Object(SPIDER_MESH_NAME);
Object leg = Object(LEG_MESH_NAME);

vec3 up = vec3(0.0f, 1.0f, 0.0f);
vec3 view_direction = vec3(0.0f, 0.0f, 1.0f);
vec3 view_position = vec3(0.0f, 0.0f, -5.0f);
vec3 right_vector = cross(view_direction, up);

GLfloat view_translate_x = 0.0f;
GLfloat view_translate_y = 0.0f;
GLfloat view_translate_z = 10.0f;

GLfloat rotate_increment = 1.0f;
GLfloat translate_increment = 0.1f;
GLfloat scale_increment = 2.0f;

GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;
GLfloat translate_x = 0.0f;
GLfloat translate_y = 0.0f;
GLfloat translate_z = 0.0f;
GLfloat scale_x = 1.0f;
GLfloat scale_y = 1.0f;
GLfloat scale_z = 1.0f;

mat4 view;
mat4 persp_proj;
mat4 model;

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);


	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");


	// Root of the Hierarchy
	view = identity_mat4();
	persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	model = identity_mat4();
	model = scale(model, vec3(scale_x, scale_y, scale_z));
	model = rotate_x_deg(model, rotate_x);
	model = rotate_y_deg(model, rotate_y);
	model = rotate_z_deg(model, rotate_z);
	model = translate(model, vec3(translate_x, translate_y, translate_z));

	view = look_at(
		view_position,
		view_position + view_direction,
		up
	);

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glBindVertexArray(spider.vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.mesh_data.mPointCount);

	glBindVertexArray(leg.vao);

	// Set up the child matrix
	mat4 modelChild = identity_mat4();
	modelChild = rotate_z_deg(modelChild, 180);
	modelChild = rotate_y_deg(modelChild, rotate_z);
	modelChild = translate(modelChild, vec3(0.0f, 1.9f, 0.0f));
	// Apply the root matrix to the child matrix
	modelChild = model * modelChild;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild.m);
	glDrawArrays(GL_TRIANGLES, 0, leg.mesh_data.mPointCount);
	glutSwapBuffers();
}


void moveCamera(GLfloat forward, GLfloat horizontal, GLfloat vertical) {
	view_position += view_direction * forward + right_vector * horizontal + up * vertical;
}

void rotateCamera(GLfloat x, GLfloat y, GLfloat z) {
	x = -x;
	y = -y;
	z = -z;
	vec3 tempDirection = view_direction;
	versor directionVersor = normalise(quat_from_axis_deg(x, up.v[0], up.v[1], up.v[2]) * quat_from_axis_deg(y, right_vector.v[0], right_vector.v[1], right_vector.v[2]) * quat_from_axis_deg(z, view_direction.v[0], view_direction.v[1], view_direction.v[2]));
	vec4 directionVec4 = quat_to_mat4(directionVersor).operator*(vec4(view_direction.v[0], view_direction.v[1], view_direction.v[2], 0.0f));
	view_direction = vec3(directionVec4.v[0], directionVec4.v[1], directionVec4.v[2]);

	versor upVersor = normalise(quat_from_axis_deg(x, up.v[0], up.v[1], up.v[2]) * quat_from_axis_deg(y, right_vector.v[0], right_vector.v[1], right_vector.v[2]) * quat_from_axis_deg(z, tempDirection.v[0], tempDirection.v[1], tempDirection.v[2]));
	vec4 upVec4 = quat_to_mat4(upVersor).operator*(vec4(up.v[0], up.v[1], up.v[2], 0.0f));
	up = vec3(upVec4.v[0], upVec4.v[1], upVec4.v[2]);

	right_vector = cross(view_direction, up);
}

void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	// Rotate the model slowly around the y axis at 20 degrees per second
	//rotate_z += 20.0f * delta;
	//rotate_z = fmodf(rotate_z, 360.0f);

	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	// load mesh into a vertex buffer array
	shaderProgramID = Shaders::CompileShaders();

	glGenVertexArrays(1, &spider.vao);
	spider.generateObjectBufferMesh(shaderProgramID);

	glGenVertexArrays(1, &leg.vao);
	leg.generateObjectBufferMesh(shaderProgramID);
	
	model = identity_mat4();
}

void mouseMoved(int newMouseX, int newMouseY) {
	rotateCamera((GLfloat)newMouseX - width / 2, 0, 0);
	glutWarpPointer(width/2, height/2);
}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case '`':
		printf("`Reset model ");
		rotate_x = 0.0f;
		rotate_y = 0.0f;
		rotate_z = 0.0f;
		translate_x = 0.0f;
		translate_y = 0.0f;
		translate_z = 0.0f;
		scale_x = 1.0f;
		scale_y = 1.0f;
		scale_z = 1.0f;
		break;
	case '1':
		printf("Rotate around x-axis ");
		rotate_x += rotate_increment;
		rotate_x = fmodf(rotate_x, 360.0f);
		printf("%f\n", rotate_x);
		break;
	case '!':
		printf("Rotate backwards around x-axis ");
		rotate_x -= rotate_increment;
		rotate_x = fmodf(rotate_x, 360.0f);
		printf("%f\n", rotate_x);
		break;
	case '2':
		printf("Rotate around y-axis ");
		rotate_y += rotate_increment;
		rotate_y = fmodf(rotate_y, 360.0f);
		printf("%f\n", rotate_y);
		break;
	case '"':
		printf("Rotate backwards around y-axis ");
		rotate_y -= rotate_increment;
		rotate_y = fmodf(rotate_y, 360.0f);
		printf("%f\n", rotate_y);
		break;
	case '3':
		printf("Rotate around z-axis ");
		rotate_z += rotate_increment;
		rotate_z = fmodf(rotate_z, 360.0f);
		printf("%f\n", rotate_z);
		break;
	case '£':
	case 163:	// the above case does not always work, so use the ascii number
		printf("Rotate backwards around z-axis ");
		rotate_z -= rotate_increment;
		rotate_z = fmodf(rotate_z, 360.0f);
		printf("%f\n", rotate_z);
		break;
	case '4':
		printf("Translate in the x-axis ");
		translate_x += translate_increment;
		printf("%f\n", translate_x);
		break;
	case '$':
		printf("Translate backwards in the x-axis ");
		translate_x -= translate_increment;
		printf("%f\n", translate_x);
		break;
	case '5':
		printf("Translate in the y-axis ");
		translate_y += translate_increment;
		printf("%f\n", translate_y);
		break;
	case '%':
		printf("Translate backwards in the y-axis ");
		translate_y -= translate_increment;
		printf("%f\n", translate_y);
		break;
	case '6':
		printf("Translate in the z-axis ");
		translate_z += translate_increment;
		printf("%f\n", translate_z);
		break;
	case '^':
		printf("Translate backwards in the z-axis ");
		translate_z -= translate_increment;
		printf("%f\n", translate_z);
		break;
	case '7':
		printf("Scale positively in the x-axis");
		scale_x *= scale_increment;
		printf("%f\n", scale_x);
		break;
	case '&':
		printf("Scale negatively in the x-axis");
		scale_x /= scale_increment;
		printf("%f\n", scale_x);
		break;
	case '8':
		printf("Scale positively in the y-axis ");
		scale_y *= scale_increment;
		printf("%f\n", scale_y);
		break;
	case '*':
		printf("Scale negatively in the y-axis");
		scale_y /= scale_increment;
		printf("%f\n", scale_y);
		break;
	case '9':
		printf("Scale in the z-axis\n");
		scale_z *= scale_increment;
		printf("%f\n", scale_z);
		break;
	case '(':
		printf("Scale negatively in the z-axis");
		scale_z /= scale_increment;
		printf("%f\n", scale_z);
		break;
	case '0':
		printf("Scale uniformly\n");
		scale_x *= scale_increment;
		scale_y *= scale_increment;
		scale_z *= scale_increment;
		printf("%f, %f, %f\n", scale_x, scale_y, scale_z);
		break;
	case ')':
		printf("Scale uniformly\n");
		scale_x /= scale_increment;
		scale_y /= scale_increment;
		scale_z /= scale_increment;
		printf("%f, %f, %f\n", scale_x, scale_y, scale_z);
		break;
	case 'W':
	case 'w':
		printf("Move camera forward ");
		//view_translate_z += translate_increment;
		moveCamera(translate_increment, 0.0f, 0.0f);
		printf("View translate z = %f\n", view_translate_z);
		break;
	case 'A':
	case 'a':
		printf("Move camera left");
		//view_translate_x += translate_increment;
		moveCamera(0.0f, -translate_increment, 0.0f);
		printf("View translate x = %f\n", view_translate_x);
		break;
	case 'S':
	case 's':
		printf("Move camera backward");
		//view_translate_z -= translate_increment;
		moveCamera(-translate_increment, 0.0f, 0.0f);
		printf("View translate z = %f\n", view_translate_z);
		break;
	case 'D':
	case 'd':
		printf("Move camera right");
		//view_translate_x -= translate_increment;
		moveCamera(0.0f, translate_increment, 0.0f);
		printf("View translate x = %f\n", view_translate_x);
		break;
	case 'Q':
	case 'q':
		glutLeaveMainLoop();
		break;		
	}
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouseMoved);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
