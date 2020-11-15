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
#include "Camera.h"

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

int width = 800;
int height = 600;

Object spider = Object(SPIDER_MESH_NAME);
Object leg = Object(LEG_MESH_NAME);
Camera camera = Camera(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f, 0.0f, 1.0f));

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

GLuint shaderProgramID;

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
	mat4 identity_matrix = identity_mat4();
	view = identity_matrix;
	persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	model = identity_mat4();
	model = scale(model, vec3(scale_x, scale_y, scale_z));
	model = rotate_x_deg(model, rotate_x);
	model = rotate_y_deg(model, rotate_y);
	model = rotate_z_deg(model, rotate_z);
	model = translate(model, vec3(translate_x, translate_y, translate_z));

	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glBindVertexArray(spider.vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.mesh_data.mPointCount);

	glBindVertexArray(leg.vao);

	
	mat4 legArray[8 * sizeof(mat4)];

	vec3 leg_scaling_factor = vec3(0.6, 0.6, 0.6);

	legArray[0] = scale(identity_matrix, leg_scaling_factor);
	legArray[0] = translate(legArray[0], vec3(0.0f, 0.0f, -1.5f));

	legArray[1] = scale(identity_matrix, leg_scaling_factor);
	legArray[1] = rotate_y_deg(legArray[1], 180);
	legArray[1] = translate(legArray[1], vec3(0.0f, 0.0f, -1.5f));

	legArray[2] = scale(identity_matrix, leg_scaling_factor);
	legArray[2] = translate(legArray[2], vec3(0.0f, 0.0f, -0.667f));

	legArray[3] = scale(identity_matrix, leg_scaling_factor);
	legArray[3] = translate(legArray[3], vec3(0.0f, 0.0f, -0.667f));
	legArray[3] = rotate_y_deg(legArray[3], 180);

	legArray[4] = scale(identity_matrix, leg_scaling_factor);
	legArray[4] = translate(legArray[4], vec3(0.0f, 0.0f, 0.167f));

	legArray[5] = scale(identity_matrix, leg_scaling_factor);
	legArray[5] = rotate_y_deg(legArray[5], 180);
	legArray[5] = translate(legArray[5] , vec3(0.0f, 0.0f, 0.167f));

	legArray[6] = scale(identity_matrix, leg_scaling_factor);
	legArray[6] = translate(legArray[6], vec3(0.0f, 0.0f, 1.0f));

	legArray[7] = scale(identity_matrix, leg_scaling_factor);
	legArray[7] = rotate_y_deg(legArray[7], 180);
	legArray[7] = translate(legArray[7], vec3(0.0f, 0.0f, 1.0f));



	for (int i = 0; i < 8; i++) {
		legArray[i] = model * legArray[i];
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, legArray[i].m);
		glDrawArrays(GL_TRIANGLES, 0, leg.mesh_data.mPointCount);
	}

	glutSwapBuffers();
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
	shaderProgramID = Shaders::CompileShaders();

	glGenVertexArrays(1, &spider.vao);
	spider.generateObjectBufferMesh(shaderProgramID);

	glGenVertexArrays(1, &leg.vao);
	leg.generateObjectBufferMesh(shaderProgramID);
	
	model = identity_mat4();
}

void mouseMoved(int newMouseX, int newMouseY) {
	camera.rotate((GLfloat)newMouseX - width / 2, 0, 0);
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
		camera.move(translate_increment, 0.0f, 0.0f);
		break;
	case 'A':
	case 'a':
		printf("Move camera left");
		camera.move(0.0f, -translate_increment, 0.0f);
		break;
	case 'S':
	case 's':
		printf("Move camera backward");
		camera.move(-translate_increment, 0.0f, 0.0f);
		break;
	case 'D':
	case 'd':
		printf("Move camera right");
		camera.move(0.0f, translate_increment, 0.0f);
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
	glutCreateWindow("Spooky Spider");

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
