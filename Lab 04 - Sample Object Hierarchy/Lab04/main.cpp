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
	Object(&specular_spider_model),
	Object(&specular_spider_model),
	Object(&specular_spider_model)
};

Object textured_box_objects[] = {
	Object(&box1),
	Object(&box2),
	Object(&box3),
	Object(&box4),
	Object(&box5),
};

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
	tile.generateVAO(diffuseShaderProgramID, "");
	specular_eye_model.generateVAO(specularShaderProgramID, "");
	specular_box.generateVAO(specularShaderProgramID, "");

	textured_box_objects[0].model->generateVAO(textureShaderProgramID, "hair_texture.jpg");
	textured_box_objects[1].model->generateVAO(textureShaderProgramID, "blue_wall.jpg");
	textured_box_objects[2].model->generateVAO(textureShaderProgramID, "fur_texture.jpg");
	textured_box_objects[3].model->generateVAO(textureShaderProgramID, "scratched_metal.jpg");
	textured_box_objects[4].model->generateVAO(textureShaderProgramID, "spooky_wood.jpg");

}

void bind_texture_shader_uniforms() {
	matrix_location = glGetUniformLocation(textureShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(textureShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(textureShaderProgramID, "proj");
}

void bind_diffuse_shader_uniforms() {
	matrix_location = glGetUniformLocation(diffuseShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(diffuseShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(diffuseShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(diffuseShaderProgramID, "object_color");
	lights_position_location = glGetUniformLocation(diffuseShaderProgramID, "lights_position");
}

void bind_specular_shader_uniforms() {
	matrix_location = glGetUniformLocation(specularShaderProgramID, "model");
	view_mat_location = glGetUniformLocation(specularShaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(specularShaderProgramID, "proj");
	object_color_location = glGetUniformLocation(specularShaderProgramID, "object_color");
	view_pos_location = glGetUniformLocation(specularShaderProgramID, "view_pos");
	specular_coef_location = glGetUniformLocation(specularShaderProgramID, "specular_coef");
	lights_position_location = glGetUniformLocation(specularShaderProgramID, "lights_position");
}

void draw_spider(vec3 spider_color, vec3 initial_coords, vec3 lights_position[]) {

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
	bind_specular_shader_uniforms();

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, spider_matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&spider_color);
	glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
	glUniform1f(specular_coef_location, 1);
	glUniform3fv(lights_position_location, 2, (GLfloat*)lights_position);
	glBindVertexArray(specular_spider_model.vao);
	glDrawArrays(GL_TRIANGLES, 0, specular_spider_model.mesh_data.mPointCount);

	mat4 identity_matrix = identity_mat4();

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
	bind_texture_shader_uniforms();

	// Update the texture uniforms and draw each leg
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	glBindTexture(GL_TEXTURE_2D, leg.texture);
	glBindVertexArray(leg.vao);

	for (int i = 0; i < 8; i++) {
		legArray[i] = scale(legArray[i], leg_scaling_factor);
		legArray[i] = spider_matrix * legArray[i];
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, legArray[i].m);
		glDrawArrays(GL_TRIANGLES, 0, leg.mesh_data.mPointCount);
	}

	vec3 eye_color = vec3(0.63, 0.13, 0.94);
	mat4 eyeArray[7 * sizeof(mat4)];
	vec3 eye_scaling_factor = vec3(0.15, 0.15, 0.15);
	glUseProgram(specularShaderProgramID);
	bind_specular_shader_uniforms();
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	glUniform3fv(object_color_location, 1, (GLfloat*)&eye_color);
	glUniform3fv(view_pos_location, 1, (GLfloat*)&camera.position);
	glUniform1f(specular_coef_location, 1);
	glUniform3fv(lights_position_location, 2, (GLfloat*)lights_position);
	glBindVertexArray(specular_eye_model.vao);

	for (int i = 0; i < 7; i++) {
		eyeArray[i] = identity_mat4();
		eyeArray[i] = scale(eyeArray[i], eye_scaling_factor);
		if(i<4)
			eyeArray[i] = translate(eyeArray[i], vec3(i*0.25 - 0.4, 0.3, 2.9));
		else
			eyeArray[i] = translate(eyeArray[i], vec3(i * 0.25 - 1.25, 0.45, 2.9));
		eyeArray[i] = spider_matrix * eyeArray[i];
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, eyeArray[i].m);
		glDrawArrays(GL_TRIANGLES, 0, specular_eye_model.mesh_data.mPointCount);
	}

}

void draw_static_scene() {
	// set up uniforms for static textured objects:
	glUseProgram(textureShaderProgramID);
	bind_texture_shader_uniforms();
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	for (int i = 0; i < 5; i++) {
		textured_box_objects[i].matrix = scale(identity_mat4(), vec3(0.2, 0.2, 0.2));
		textured_box_objects[i].matrix = translate(textured_box_objects[i].matrix, vec3(i, 0.5, 20));
		glBindTexture(GL_TEXTURE_2D, textured_box_objects[i].model->texture);
		glBindVertexArray(textured_box_objects[i].model->vao);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, textured_box_objects[i].matrix.m);
		glDrawArrays(GL_TRIANGLES, 0, textured_box_objects[i].model->mesh_data.mPointCount);
	}
}

void display() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Object colors
	vec3 leg_color = vec3(0.5, 0.5, 0.5);
	vec3 floor_color = vec3(1, 1, 1);

	vec3 lights_position[] = {
		vec3(4, 1, 20),
		vec3(-4, 1, 20)
	};

	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
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
	bind_specular_shader_uniforms();

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
	bind_diffuse_shader_uniforms();

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