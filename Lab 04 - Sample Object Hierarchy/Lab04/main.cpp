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
#include "Model.h"
#include "Shaders.h"
#include "Camera.h"

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define SPIDER_MESH_NAME "spider.dae"
#define LEG_MESH_NAME "leg.dae"
#define TILE_MESH_NAME "tile.dae"

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
using namespace std;

int width = 800;
int height = 600;

Model spider = Model(SPIDER_MESH_NAME);
Model leg = Model(LEG_MESH_NAME);
Model tile = Model(TILE_MESH_NAME);
Camera camera = Camera(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

GLfloat rotate_increment = 1.0f;
GLfloat translate_increment = 0.1f;
GLfloat scale_increment = 2.0f;

GLfloat perspective_fovy = 45.0f;

GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;
GLfloat translate_x = 0.0f;
GLfloat translate_y = 0.0f;
GLfloat translate_z = 0.0f;
GLfloat scale_x = 1.0f;
GLfloat scale_y = 1.0f;
GLfloat scale_z = 1.0f;


GLfloat leg_set_1_rotate_x = 0.0f;
boolean leg_set_1_rotate_x_increasing = true;

GLfloat leg_set_2_rotate_x = 0.0f;
boolean leg_set_2_rotate_x_increasing = false;

boolean animation = false;
boolean mouseInput = false;

int matrix_location;
int view_mat_location;
int proj_mat_location;

int light_position_location;
int object_color_location;

int view_pos_location;
int specular_coef_location;

mat4 view;
mat4 persp_proj;
mat4 model;



void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Root of the Hierarchy
	mat4 identity_matrix = identity_mat4();
	view = identity_matrix;
	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
	model = identity_mat4();
	model = scale(model, vec3(scale_x, scale_y, scale_z));
	model = rotate_x_deg(model, rotate_x);
	model = rotate_y_deg(model, 180);
	model = rotate_y_deg(model, rotate_y);
	model = rotate_z_deg(model, rotate_z);
	model = translate(model, vec3(0.0f, 1.4f, 10.0f));
	model = translate(model, vec3(translate_x, translate_y, translate_z));

	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);

	glUniform3fv(view_pos_location, 1, (GLfloat *)&camera.position);
	glUniform1f(specular_coef_location, 100);

	vec3 spider_color = vec3(1.0, 0.0, 0.0);
	vec3 leg_color = vec3(0.5, 0.5, 0.5);
	vec3 light1_position = vec3(10, 0, 10);

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glUniform3fv(light_position_location, 1, (GLfloat*)&light1_position);




	glUniform3fv(object_color_location, 1, (GLfloat*)&spider_color);
	glBindVertexArray(spider.vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.mesh_data.mPointCount);

	glUniform3fv(object_color_location, 1, (GLfloat*)&leg_color);
	glBindVertexArray(leg.vao);
	mat4 legArray[8 * sizeof(mat4)];
	vec3 leg_scaling_factor = vec3(0.6, 0.6, 0.6);

	legArray[0] = scale(identity_matrix, leg_scaling_factor);
	legArray[0] = rotate_x_deg(legArray[0], leg_set_1_rotate_x);
	legArray[0] = translate(legArray[0], vec3(0.0f, 0.0f, -1.5f));

	legArray[1] = scale(identity_matrix, leg_scaling_factor);
	legArray[1] = rotate_y_deg(legArray[1], 180);
	legArray[1] = rotate_x_deg(legArray[1], leg_set_2_rotate_x);
	legArray[1] = translate(legArray[1], vec3(0.0f, 0.0f, -1.5f));

	legArray[2] = scale(identity_matrix, leg_scaling_factor);
	legArray[2] = rotate_x_deg(legArray[2], leg_set_2_rotate_x);
	legArray[2] = translate(legArray[2], vec3(0.0f, 0.0f, -0.667f));

	legArray[3] = scale(identity_matrix, leg_scaling_factor);
	legArray[3] = rotate_y_deg(legArray[3], 180);
	legArray[3] = rotate_x_deg(legArray[3], leg_set_1_rotate_x);
	legArray[3] = translate(legArray[3], vec3(0.0f, 0.0f, -0.667f));

	legArray[4] = scale(identity_matrix, leg_scaling_factor);
	legArray[4] = rotate_x_deg(legArray[4], leg_set_1_rotate_x);
	legArray[4] = translate(legArray[4], vec3(0.0f, 0.0f, 0.167f));

	legArray[5] = scale(identity_matrix, leg_scaling_factor);
	legArray[5] = rotate_y_deg(legArray[5], 180);
	legArray[5] = rotate_x_deg(legArray[5], leg_set_2_rotate_x);
	legArray[5] = translate(legArray[5] , vec3(0.0f, 0.0f, 0.167f));

	legArray[6] = scale(identity_matrix, leg_scaling_factor);
	legArray[6] = rotate_x_deg(legArray[6], leg_set_2_rotate_x);
	legArray[6] = translate(legArray[6], vec3(0.0f, 0.0f, 1.0f));

	legArray[7] = scale(identity_matrix, leg_scaling_factor);
	legArray[7] = rotate_y_deg(legArray[7], 180);
	legArray[7] = rotate_x_deg(legArray[7], leg_set_1_rotate_x);
	legArray[7] = translate(legArray[7], vec3(0.0f, 0.0f, 1.0f));


	for (int i = 0; i < 8; i++) {
		legArray[i] = model * legArray[i];
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, legArray[i].m);
		glDrawArrays(GL_TRIANGLES, 0, leg.mesh_data.mPointCount);
	}

	
	glBindVertexArray(tile.vao);
	mat4 floor_matrix = identity_matrix;
	floor_matrix = scale(floor_matrix, vec3(100, 0, 100));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, floor_matrix.m);
	glDrawArrays(GL_TRIANGLES, 0, tile.mesh_data.mPointCount);
	


	glutSwapBuffers();
}

void update_leg_rotation(GLfloat &leg_rotation, boolean &increasing, float delta) {
	if (increasing) {
		leg_rotation += (20.0f * delta);
		if (leg_rotation > 5.0f) {
			increasing = false;
		}
	}
	else {
		leg_rotation -= (20.0f * delta);
		if (leg_rotation < -5.0f) {
			increasing = true;
		}
	}
}

void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	if (animation) {
		update_leg_rotation(leg_set_1_rotate_x, leg_set_1_rotate_x_increasing, delta);
		update_leg_rotation(leg_set_2_rotate_x, leg_set_2_rotate_x_increasing, delta);
	}
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	GLuint diffuseShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "diffuseFragmentShader.txt");
	GLuint specularShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "specularFragmentShader.txt");
	glUseProgram(diffuseShaderProgramID);
	//Declare your uniform variables that will be used in your shader
	matrix_location = glGetUniformLocation(diffuseShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(diffuseShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(diffuseShaderProgramID, "proj");
	light_position_location = glGetUniformLocation(diffuseShaderProgramID, "light_position");
	object_color_location = glGetUniformLocation(diffuseShaderProgramID, "object_color");

	spider.generateVAO(diffuseShaderProgramID);
	leg.generateVAO(diffuseShaderProgramID);
	tile.generateVAO(diffuseShaderProgramID);
	model = identity_mat4();

	//
	glUseProgram(specularShaderProgramID);
	//Declare your uniform variables that will be used in your shader
	matrix_location = glGetUniformLocation(specularShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(specularShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(specularShaderProgramID, "proj");
	light_position_location = glGetUniformLocation(specularShaderProgramID, "light_position");
	object_color_location = glGetUniformLocation(specularShaderProgramID, "object_color");

	view_pos_location = glGetUniformLocation(specularShaderProgramID, "view_pos");
	specular_coef_location = glGetUniformLocation(specularShaderProgramID, "specular_coef");

	spider.generateVAO(specularShaderProgramID);
	leg.generateVAO(specularShaderProgramID);
	tile.generateVAO(specularShaderProgramID);
	model = identity_mat4();
}

void mouseMoved(int newMouseX, int newMouseY) {
	if (mouseInput) {
		printf("MOUSE MOVED - Rotate camera. \n");
		camera.rotate((GLfloat)newMouseX - width / 2, 0, 0);
		glutWarpPointer(width / 2, height / 2);
	}
}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case '`':
		printf("PRESSED ` - Reset model.\n");
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
		printf("PRESSED 1 - Rotate spider forwards.\n");
		rotate_x += rotate_increment;
		rotate_x = fmodf(rotate_x, 360.0f);
		break;
	case '!':
		printf("PRESSED SHIFT 1 - Rotate spider backwards. \n");
		rotate_x -= rotate_increment;
		rotate_x = fmodf(rotate_x, 360.0f);
		break;
	case '2':
		printf("PRESSED 2 - Rotate spider left. \n");
		rotate_y += rotate_increment;
		rotate_y = fmodf(rotate_y, 360.0f);
		break;
	case '"':
		printf("PRESSED SHIFT 2 - Rotate spider right. \n");
		rotate_y -= rotate_increment;
		rotate_y = fmodf(rotate_y, 360.0f);
		break;
	case '3':
		printf("PRESSED 3 - Rotate spider clockwise. \n");
		rotate_z += rotate_increment;
		rotate_z = fmodf(rotate_z, 360.0f);
		break;
	case '£':
	case 163:	// the above case does not always work, so use the ascii number
		printf("PRESSED SHIFT 3 - Rotate spider counter-clockwise. \n");
		rotate_z -= rotate_increment;
		rotate_z = fmodf(rotate_z, 360.0f);
		break;
	case '4':
		printf("PRESSED 4 - Translate spider positively on x-axis. \n");
		translate_x += translate_increment;
		break;
	case '$':
		printf("PRESSED SHIFT 4 - Translate spider negatively on x-axis. \n");
		translate_x -= translate_increment;
		break;
	case '5':
		printf("PRESSED 5 - Translate spider positively on y-axis. \n");
		translate_y += translate_increment;
		break;
	case '%':
		printf("PRESSED SHIFT 5 - Translate spider negatively on y-axis. \n");
		translate_y -= translate_increment;
		break;
	case '6':
		printf("PRESSED 6 - Translate spider positively on z-axis. \n");
		translate_z += translate_increment;
		break;
	case '^':
		printf("PRESSED 6 - Translate spider negatviely on z-axis. \n");
		translate_z -= translate_increment;
		break;
	case '7':
		printf("PRESSED 7 - Increase spider width. \n");
		scale_x *= scale_increment;
		break;
	case '&':
		printf("PRESSED SHIFT 7 - Decrease spider width. \n");
		scale_x /= scale_increment;
		break;
	case '8':
		printf("PRESSED 8 - Increase spider height. \n");
		scale_y *= scale_increment;
		break;
	case '*':
		printf("PRESSED SHIFT 8 - Decrease spider height. \n");
		scale_y /= scale_increment;
		break;
	case '9':
		printf("PRESSED 9 - Increase spider depth. \n");
		scale_z *= scale_increment;
		break;
	case '(':
		printf("PRESSED SHIFT 9 - Decrease spider depth. \n");
		scale_z /= scale_increment;
		break;
	case '0':
		printf("PRESSED 0 - Increase spider size uniformly. \n");
		scale_x *= scale_increment;
		scale_y *= scale_increment;
		scale_z *= scale_increment;
		break;
	case ')':
		printf("PRESSED SHIFT 0 - Decrease spider size uniformly. \n");
		scale_x /= scale_increment;
		scale_y /= scale_increment;
		scale_z /= scale_increment;
		break;
	case 'W':
	case 'w':
		printf("PRESSED W - Walk forward.\n");
		camera.move(translate_increment, 0.0f, 0.0f);
		break;
	case 'A':
	case 'a':
		printf("PRESSED A - Walk left.\n");
		camera.move(0.0f, -translate_increment, 0.0f);
		break;
	case 'S':
	case 's':
		printf("PRESSED S - Walk backward.\n");
		camera.move(-translate_increment, 0.0f, 0.0f);
		break;
	case 'D':
	case 'd':
		printf("PRESSED D - Walk right.\n");
		camera.move(0.0f, translate_increment, 0.0f);
		break;
	case 'E':
	case 'e':
		printf("PRESSED E - Toggled animation. \n");
		animation = !animation;
		break;
	case 'm':
	case 'M':
		printf("PRESSED M - Toggled mouse input / mouse binding. \n");
		mouseInput = !mouseInput;
		break;
	case 'p':
	case 'P':
		printf("Switched perspective FOV. \n");
		if (perspective_fovy == 30.0f) {
			perspective_fovy = 45.0f;
		}
		else if (perspective_fovy == 45.0f) {
			perspective_fovy = 60.0f;
		}
		else {
			perspective_fovy = 30.0f;
		}
		break;
	case 'Q':
	case 'q':
		printf("PRESSED Q - QUIT. \n");
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
