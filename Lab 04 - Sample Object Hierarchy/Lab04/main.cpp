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
#define CUBE_MESH_NAME "cube.dae"

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
using namespace std;

int width = 800;
int height = 600;

Model spider = Model(SPIDER_MESH_NAME);
Model leg = Model(LEG_MESH_NAME);
Model tile = Model(TILE_MESH_NAME);
Model specular_box = Model(CUBE_MESH_NAME);

Model textured_boxes[] = {
	Model(CUBE_MESH_NAME),
	Model(CUBE_MESH_NAME),
	Model(CUBE_MESH_NAME),
	Model(CUBE_MESH_NAME),
	Model(CUBE_MESH_NAME),
	Model(CUBE_MESH_NAME)
};


Model hair_box = Model(CUBE_MESH_NAME);
Model scratched_box = Model(CUBE_MESH_NAME);


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
GLfloat scale_x = 0.1f;
GLfloat scale_y = 0.1f;
GLfloat scale_z = 0.1f;


GLfloat leg_set_1_rotate_x = 0.0f;
boolean leg_set_1_rotate_x_increasing = true;

GLfloat leg_set_2_rotate_x = 0.0f;
boolean leg_set_2_rotate_x_increasing = false;

boolean animation = true;
boolean mouseInput = false;

GLfloat light_x = 4;
GLfloat light_y = 1;
GLfloat light_z = 20;

int matrix_location;
int view_mat_location;
int proj_mat_location;

int object_color_location;

int view_pos_location;
int specular_coef_location;

int lights_position_location;

mat4 view;
mat4 persp_proj;

GLuint diffuseShaderProgramID;
GLuint specularShaderProgramID;
GLuint textureShaderProgramID;

//enum shader_types {AMBIENT, DIFFUSE, SPECULAR, TEXTURE};
void draw_spider(vec3 spider_color, vec3 initial_coords, vec3 lights_position[]) {
	// Update the root of the hierarchical spider model
	mat4 identity_matrix = identity_mat4();

	mat4 spider_matrix = identity_mat4();
	spider_matrix = scale(spider_matrix, vec3(scale_x, scale_y, scale_z));
	spider_matrix = rotate_x_deg(spider_matrix, rotate_x);
	spider_matrix = rotate_y_deg(spider_matrix, 180);
	spider_matrix = rotate_y_deg(spider_matrix, rotate_y);
	spider_matrix = rotate_z_deg(spider_matrix, rotate_z);
	spider_matrix = translate(spider_matrix, initial_coords);
	spider_matrix = translate(spider_matrix, vec3(translate_x, translate_y, translate_z));


	// Activate the specular shader program and locate the uniforms
	glUseProgram(specularShaderProgramID);
	matrix_location = glGetUniformLocation(specularShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(specularShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(specularShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(specularShaderProgramID, "object_color");
	view_pos_location = glGetUniformLocation(specularShaderProgramID, "view_pos");
	specular_coef_location = glGetUniformLocation(specularShaderProgramID, "specular_coef");

	lights_position_location = glGetUniformLocation(specularShaderProgramID, "lights_position");

	// Update the specular uniforms and draw the spider
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, spider_matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&spider_color);
	glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
	glUniform1f(specular_coef_location, 1);

	glUniform3fv(lights_position_location, 2, (GLfloat*)lights_position);

	glBindVertexArray(spider.vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.mesh_data.mPointCount);


	// Update the matrix for each leg model
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
	legArray[5] = translate(legArray[5], vec3(0.0f, 0.0f, 0.167f));

	legArray[6] = scale(identity_matrix, leg_scaling_factor);
	legArray[6] = rotate_x_deg(legArray[6], leg_set_2_rotate_x);
	legArray[6] = translate(legArray[6], vec3(0.0f, 0.0f, 1.0f));

	legArray[7] = scale(identity_matrix, leg_scaling_factor);
	legArray[7] = rotate_y_deg(legArray[7], 180);
	legArray[7] = rotate_x_deg(legArray[7], leg_set_1_rotate_x);
	legArray[7] = translate(legArray[7], vec3(0.0f, 0.0f, 1.0f));

	// Activate the texture shader program and locate the uniforms
	glUseProgram(textureShaderProgramID);
	matrix_location = glGetUniformLocation(textureShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(textureShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(textureShaderProgramID, "proj");

	// Update the texture uniforms and draw each leg
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	glBindTexture(GL_TEXTURE_2D, leg.texture);
	glBindVertexArray(leg.vao);

	for (int i = 0; i < 8; i++) {
		legArray[i] = spider_matrix * legArray[i];
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, legArray[i].m);
		glDrawArrays(GL_TRIANGLES, 0, leg.mesh_data.mPointCount);
	}
}

void draw_static_scene() {

	// set up uniforms for static textured objects:
	glUseProgram(textureShaderProgramID);
	matrix_location = glGetUniformLocation(textureShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(textureShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(textureShaderProgramID, "proj");
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	// draw static textured boxes
	for (int i = 0; i < 5; i++) {
		mat4 textured_box_i = identity_mat4();
		textured_box_i = scale(textured_box_i, vec3(0.2, 0.2, 0.2));
		textured_box_i = translate(textured_box_i, vec3(i, 0.5, 20));
		glBindTexture(GL_TEXTURE_2D, textured_boxes[i].texture);
		glBindVertexArray(textured_boxes[i].vao);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, textured_box_i.m);
		glDrawArrays(GL_TRIANGLES, 0, textured_boxes[i].mesh_data.mPointCount);

	}


}


void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Object colors
	vec3 leg_color = vec3(0.5, 0.5, 0.5);
	vec3 floor_color = vec3(1, 1, 1);
	vec3 light1_position = vec3(light_x, light_y, light_z);

	vec3 lights_position[] = {
		vec3(4, 1, 20),
		vec3(-4, 1, 20)
	};

	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
	// Update the camera
	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);


	draw_static_scene();

	draw_spider(vec3(1, 0, 0), vec3(-1.0f, 0.5f, 20.0f), lights_position);
	draw_spider(vec3(0, 1, 0), vec3(0.0f, 0.5f, 25.0f), lights_position);
	draw_spider(vec3(0, 0, 1), vec3(1.0f, 0.5f, 20.0f), lights_position);

	glUseProgram(specularShaderProgramID);
	matrix_location = glGetUniformLocation(specularShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(specularShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(specularShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(specularShaderProgramID, "object_color");
	view_pos_location = glGetUniformLocation(specularShaderProgramID, "view_pos");
	specular_coef_location = glGetUniformLocation(specularShaderProgramID, "specular_coef");

	lights_position_location = glGetUniformLocation(specularShaderProgramID, "lights_position");



	// draw lights
	for (int i = 0; i < 2; i++) {
		mat4 light_matrix = identity_mat4();
		light_matrix = scale(light_matrix, vec3(0.1, 0.1, 0.1));
		light_matrix = translate(light_matrix, lights_position[i]);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, light_matrix.m);

		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

		glUniform3fv(object_color_location, 1, (GLfloat*)&vec3(1, 1, 1));
		glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
		glUniform1f(specular_coef_location, 200);

		glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);


		glBindVertexArray(specular_box.vao);
		glDrawArrays(GL_TRIANGLES, 0, specular_box.mesh_data.mPointCount);
	}
	
	// Update the matrix for the floor model
	mat4 floor_matrix = identity_mat4();
	floor_matrix = scale(floor_matrix, vec3(100.0f, 1.0f, 100.0f));

	// Activate the diffuse shader program and locate the uniforms
	glUseProgram(diffuseShaderProgramID);
	matrix_location = glGetUniformLocation(diffuseShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(diffuseShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(diffuseShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(diffuseShaderProgramID, "object_color");
	lights_position_location = glGetUniformLocation(diffuseShaderProgramID, "lights_position");

	
	// Update the diffuse uniforms and draw the floor
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, floor_matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&floor_color);

	glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);

	glBindVertexArray(tile.vao);
	glDrawArrays(GL_TRIANGLES, 0, tile.mesh_data.mPointCount);

	glutSwapBuffers();
}

void update_leg_rotation(GLfloat &leg_rotation, boolean &increasing, float delta) {
	if (increasing) {
		leg_rotation += (20.0f * delta);
		if (leg_rotation > 10.0f) {
			increasing = false;
		}
	}
	else {
		leg_rotation -= (20.0f * delta);
		if (leg_rotation < -10.0f) {
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
		translate_z -= 0.001;
		translate_x = sin(translate_z)/2;
	}
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	diffuseShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "diffuseFragmentShader.txt");
	specularShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "specularFragmentShader.txt");
	textureShaderProgramID = Shaders::CompileShaders("textureVertexShader.txt", "textureFragmentShader.txt");

	spider.generateVAO(specularShaderProgramID, "");
	leg.generateVAO(textureShaderProgramID, "hair_texture.jpg");
	tile.generateVAO(diffuseShaderProgramID, "");
	specular_box.generateVAO(specularShaderProgramID, "");
	hair_box.generateVAO(textureShaderProgramID, "hair_texture.jpg");

	textured_boxes[0].generateVAO(textureShaderProgramID, "hair_texture.jpg");
	textured_boxes[1].generateVAO(textureShaderProgramID, "blue_wall.jpg");
	textured_boxes[2].generateVAO(textureShaderProgramID, "fur_texture.jpg");
	textured_boxes[3].generateVAO(textureShaderProgramID, "scratched_metal.jpg");
	textured_boxes[4].generateVAO(textureShaderProgramID, "spooky_wood.jpg");
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
	case 'x':
		light_x-=translate_increment;
		break;
	case 'X':
		light_x+=translate_increment;
		break;
	case 'y':
		light_y-=translate_increment;
		break;
	case 'Y':
		light_y+=translate_increment;
		break;
	case 'z':
		light_z-=translate_increment;
		break;
	case 'Z':
		light_z+=translate_increment;
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
