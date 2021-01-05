#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <assimp/cimport.h> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 

#include "maths_funcs.h"
#include "Model.h"
#include "Shaders.h"
#include "Camera.h"
#include "Object.h"
#include "Spider.h"

#define SPIDER_MESH_NAME "spider_no_eyes.dae"
#define LEG_MESH_NAME "leg.dae"
#define TILE_MESH_NAME "tile.dae"
#define CUBE_MESH_NAME "cube.dae"
#define EYE_MESH_NAME "eye.dae"

using namespace std;

int width = 800;
int height = 600;

Model specular_spider_model = Model(SPIDER_MESH_NAME);
Model specular_eye_model = Model(EYE_MESH_NAME);

Model leg = Model(LEG_MESH_NAME);
Model tile = Model(TILE_MESH_NAME);
Model specular_box = Model(CUBE_MESH_NAME);

Model box1(CUBE_MESH_NAME);
Model box2(CUBE_MESH_NAME);
Model box3(CUBE_MESH_NAME);
Model box4(CUBE_MESH_NAME);
Model box5(CUBE_MESH_NAME);

Object spider_objects[] = {
	Object(&specular_spider_model, vec3(1,0,0)),
	Object(&specular_spider_model, vec3(0,1,0)),
	Object(&specular_spider_model, vec3(0,0,1))
};

Object textured_box_objects[] = {
	Object(&box1, vec3(0,0,0)),
	Object(&box2, vec3(0,0,0)),
	Object(&box3, vec3(0,0,0)),
	Object(&box4, vec3(0,0,0)),
	Object(&box5, vec3(0,0,0)),
};

vec3 lights_position[] = {
	vec3(4, 1, 20),
	vec3(-4, 1, 20)
};

Spider spider1 = Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));

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

void init()
{
	diffuseShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "diffuseFragmentShader.txt");
	specularShaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "specularFragmentShader.txt");
	textureShaderProgramID = Shaders::CompileShaders("textureVertexShader.txt", "textureFragmentShader.txt");

	specular_spider_model.generateVAO(specularShaderProgramID, "");
	leg.generateVAO(textureShaderProgramID, "hair_texture.jpg");
	specular_eye_model.generateVAO(specularShaderProgramID, "");

	tile.generateVAO(diffuseShaderProgramID, "");
	specular_box.generateVAO(specularShaderProgramID, "");

	textured_box_objects[0].model->generateVAO(textureShaderProgramID, "hair_texture.jpg");
	textured_box_objects[1].model->generateVAO(textureShaderProgramID, "blue_wall.jpg");
	textured_box_objects[2].model->generateVAO(textureShaderProgramID, "fur_texture.jpg");
	textured_box_objects[3].model->generateVAO(textureShaderProgramID, "scratched_metal.jpg");
	textured_box_objects[4].model->generateVAO(textureShaderProgramID, "spooky_wood.jpg");

	spider1.scale_hierarchy(vec3(0.1, 0.1, 0.1));
	spider1.rotate_hierarchy_y_deg(180);
	spider1.translate_hierarchy(vec3(-1.5, 0.5, 20));

}

void activate_texture_shader() {
	glUseProgram(textureShaderProgramID);
	matrix_location = glGetUniformLocation(textureShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(textureShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(textureShaderProgramID, "proj");

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
}

void activate_diffuse_shader() {
	glUseProgram(diffuseShaderProgramID);
	matrix_location = glGetUniformLocation(diffuseShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(diffuseShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(diffuseShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(diffuseShaderProgramID, "object_color");
	lights_position_location = glGetUniformLocation(diffuseShaderProgramID, "lights_position");

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
}

void activate_specular_shader() {
	glUseProgram(specularShaderProgramID);
	matrix_location = glGetUniformLocation(specularShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(specularShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(specularShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(specularShaderProgramID, "object_color");
	view_pos_location = glGetUniformLocation(specularShaderProgramID, "view_pos");
	specular_coef_location = glGetUniformLocation(specularShaderProgramID, "specular_coef");
	lights_position_location = glGetUniformLocation(specularShaderProgramID, "lights_position");

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

}

void draw_spider(Spider spider) {

	spider.body.matrix = translate(spider.body.matrix, vec3(0, 0, 0));
	activate_specular_shader();
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, spider.body.matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&spider.body_color);
	glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
	glUniform1f(specular_coef_location, 1);
	glUniform3fv(lights_position_location, 2, (GLfloat*)lights_position);
	glBindVertexArray(spider.body.model->vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.body.model->mesh_data.mPointCount);

	glBindVertexArray(spider.body.model->vao);
	glDrawArrays(GL_TRIANGLES, 0, spider.body.model->mesh_data.mPointCount);

	glUniform3fv(object_color_location, 1, (GLfloat*)&spider.eye_color);
	glBindVertexArray(spider.eyes->model->vao);
	for (int i = 0; i < 7; i++) {
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, spider.eyes[i].matrix.m);
		glDrawArrays(GL_TRIANGLES, 0, spider.eyes[i].model->mesh_data.mPointCount);
	}


	activate_texture_shader();
	glBindTexture(GL_TEXTURE_2D, spider.legs->model->texture);
	glBindVertexArray(spider.legs->model->vao);

	for (int i = 0; i < 8; i++) {
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, spider.legs[i].matrix.m);
		glDrawArrays(GL_TRIANGLES, 0, spider.legs[i].model->mesh_data.mPointCount);
	}

}

void draw_static_scene() {
	// draw boxes
	activate_texture_shader();
	for (int i = 0; i < 5; i++) {
		textured_box_objects[i].matrix = scale(identity_mat4(), vec3(0.2, 0.2, 0.2));
		textured_box_objects[i].matrix = translate(textured_box_objects[i].matrix, vec3(i, 0.5, 20));
		glBindTexture(GL_TEXTURE_2D, textured_box_objects[i].model->texture);
		glBindVertexArray(textured_box_objects[i].model->vao);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, textured_box_objects[i].matrix.m);
		glDrawArrays(GL_TRIANGLES, 0, textured_box_objects[i].model->mesh_data.mPointCount);
	}

	//draw floor
	vec3 floor_color = vec3(1, 1, 1);
	mat4 floor_matrix = identity_mat4();
	floor_matrix = scale(floor_matrix, vec3(100.0f, 1.0f, 100.0f));
	activate_diffuse_shader();
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, floor_matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&floor_color);
	glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);
	glBindVertexArray(tile.vao);
	glDrawArrays(GL_TRIANGLES, 0, tile.mesh_data.mPointCount);

	// draw lights
	activate_specular_shader();
	for (int i = 0; i < 2; i++) {
		mat4 light_matrix = identity_mat4();
		light_matrix = scale(light_matrix, vec3(0.1, 0.1, 0.1));
		light_matrix = translate(light_matrix, lights_position[i]);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, light_matrix.m);
		glUniform3fv(object_color_location, 1, (GLfloat*)&vec3(1, 1, 1));
		glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
		glUniform1f(specular_coef_location, 200);
		glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);
		glBindVertexArray(specular_box.vao);
		glDrawArrays(GL_TRIANGLES, 0, specular_box.mesh_data.mPointCount);
	}
}

void display() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);

	draw_static_scene();
	draw_spider(spider1);

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
		spider1.translate_hierarchy(vec3(0, 0, -0.001));

	}
	glutPostRedisplay();
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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Spooky Spider");

	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouseMoved);

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	init();
	glutMainLoop();
	return 0;
}