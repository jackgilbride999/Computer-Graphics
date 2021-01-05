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

Model box6(CUBE_MESH_NAME);
Model box7(CUBE_MESH_NAME);
Model box8(CUBE_MESH_NAME);
Model box9(CUBE_MESH_NAME);
Model box10(CUBE_MESH_NAME);

Object textured_box_objects[] = {
	Object(&box1, vec3(0,0,0)),
	Object(&box2, vec3(0,0,0)),
	Object(&box3, vec3(0,0,0)),
	Object(&box4, vec3(0,0,0)),
	Object(&box5, vec3(0,0,0)),
	Object(&box6, vec3(0,0,0)),
	Object(&box7, vec3(0,0,0)),
	Object(&box8, vec3(0,0,0)),
	Object(&box9, vec3(0,0,0)),
	Object(&box10, vec3(0,0,0)),
};

Object floor_object = Object(&tile, vec3(1, 1, 1));
Object ceiling_object = Object(&tile, vec3(0.2, 0.2, 0.2));
Object walls[] = {
	Object(&tile, vec3(1.0,1.0,1.0)),
	Object(&tile, vec3(0.2,0.2,0.2)),
	Object(&tile, vec3(0.2,0.2,0.2)),
	Object(&tile, vec3(0.2,0.2,0.2))
};

vec3 lights_position[] = {
	vec3(4, 1, 0),
	vec3(-4, 1, 0)
};

Object lights[] = {
	Object(&specular_box, vec3(1,1,1)),
	Object(&specular_box, vec3(1,1,1))
};

Spider specular_spiders[] = {
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.9,0.1,0.9), vec3(0,1,0), vec3(0.0, 0.0, 0.0)),
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.5,1.0,0.0), vec3(0,1,0), vec3(0.1,1.0,0.8)),
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.1,0.4,1.0), vec3(0,1,0), vec3(1.0,1.0,1.0)),
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.8,0.1,1.0), vec3(0,1,0), vec3(0.1,0.2,1.0)),
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.8,1.0,0.1), vec3(0,1,0), vec3(1.0,0.6,0.0)),
	Spider(&specular_spider_model, &leg, &specular_eye_model, vec3(0.1,1.0,0.9), vec3(0,1,0), vec3(0.1,1.0,0.4))
};


Camera camera = Camera(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

GLfloat rotate_increment = 1.0f;
GLfloat translate_increment = 0.1f;
GLfloat scale_increment = 2.0f;

GLfloat perspective_fovy = 45.0f;

boolean animation = true;
boolean mouseInput = false;

int matrix_location, view_mat_location, proj_mat_location;
int object_color_location, lights_position_location;
int view_pos_location, specular_coef_location;
mat4 view, persp_proj;
GLuint diffuseShaderProgramID, specularShaderProgramID, textureShaderProgramID;

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
	textured_box_objects[5].model->generateVAO(textureShaderProgramID, "orange_webs.JPG");
	textured_box_objects[6].model->generateVAO(textureShaderProgramID, "green.JPEG");
	textured_box_objects[7].model->generateVAO(textureShaderProgramID, "white_webs.jpg");
	textured_box_objects[8].model->generateVAO(textureShaderProgramID, "blood.jpg");
	textured_box_objects[9].model->generateVAO(textureShaderProgramID, "smoke.jpg");

	for (int i = 0; i < 6; i++) {
		specular_spiders[i].scale_hierarchy(vec3(0.1, 0.1, 0.1));
		specular_spiders[i].rotate_hierarchy_y_deg(180);
		specular_spiders[i].translate_hierarchy(vec3(-1.5 + i, 0.5, 15 + (i % 3)));
	}

	for (int i = 0; i < 10; i++) {
		textured_box_objects[i].matrix = scale(identity_mat4(), vec3((i+1)*0.2, (i + 1) * 0.2, 0.5));
		textured_box_objects[i].matrix = rotate_y_deg(textured_box_objects[i].matrix, 60);
		if (i % 2) {
			textured_box_objects[i].matrix = translate(textured_box_objects[i].matrix, vec3(2* -i, 0.5, 20 + i * 1.5));

		}
		else {
			textured_box_objects[i].matrix = translate(textured_box_objects[i].matrix, vec3(2 * i, 0.5, 20 + i * 1.5));

		}
	}

	floor_object.matrix = scale(identity_mat4(), vec3(100.0f, 1.0f, 100.0f));
	ceiling_object.matrix = translate(floor_object.matrix, vec3(0.0f, 10.0f, 0.0f));

	walls[0].matrix = scale(identity_mat4(), vec3(200, 1.0f, 50.0f));
	walls[0].matrix = rotate_x_deg(walls[0].matrix, 90);
	walls[0].matrix = translate(walls[0].matrix, vec3(0, 0, 150));

	walls[1].matrix = translate(walls[0].matrix, vec3(0, 0, -300));

	walls[2].matrix = scale(identity_mat4(), vec3(200.0f, 1.0f, 50.0f));
	walls[2].matrix = rotate_x_deg(walls[2].matrix, 90);
	walls[2].matrix = rotate_y_deg(walls[2].matrix, 90);
	walls[2].matrix = translate(walls[2].matrix, vec3(150, 0, 0));
	walls[3].matrix = translate(walls[2].matrix, vec3(-300, 0, 0));


	lights[0].matrix = scale(identity_mat4(), vec3(0.1, 0.1, 0.1));
	lights[0].matrix = translate(lights[0].matrix, lights_position[0]);
	lights[1].matrix = scale(identity_mat4(), vec3(0.1, 0.1, 0.1));
	lights[1].matrix = translate(lights[1].matrix, lights_position[1]);
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
	glUniform1f(specular_coef_location, 0.5);
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
	for (int i = 0; i < 10; i++) {
		glBindTexture(GL_TEXTURE_2D, textured_box_objects[i].model->texture);
		glBindVertexArray(textured_box_objects[i].model->vao);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, textured_box_objects[i].matrix.m);
		glDrawArrays(GL_TRIANGLES, 0, textured_box_objects[i].model->mesh_data.mPointCount);
	}

	//draw floor
	activate_diffuse_shader();
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, floor_object.matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&floor_object.color);
	glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);
	glBindVertexArray(floor_object.model->vao);
	glDrawArrays(GL_TRIANGLES, 0, floor_object.model->mesh_data.mPointCount);

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, ceiling_object.matrix.m);
	glUniform3fv(object_color_location, 1, (GLfloat*)&ceiling_object.color);
	glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);
	glBindVertexArray(ceiling_object.model->vao);
	glDrawArrays(GL_TRIANGLES, 0, ceiling_object.model->mesh_data.mPointCount);

	for (int i = 0; i < 4; i++) {
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, walls[i].matrix.m);
		glUniform3fv(object_color_location, 1, (GLfloat*)&walls[i].color);
		glUniform3fv(lights_position_location, 2, (GLfloat*)&lights_position);
		glBindVertexArray(walls[i].model->vao);
		glDrawArrays(GL_TRIANGLES, 0, walls[i].model->mesh_data.mPointCount);
	}

	// draw lights
	activate_specular_shader();
	for (int i = 0; i < 2; i++) {
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, lights[i].matrix.m);
		glUniform3fv(object_color_location, 1, (GLfloat*)&lights[i].color);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);

	draw_static_scene();
	for(int i = 0; i < 6; i++){
		draw_spider(specular_spiders[i]);
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

	if (animation) {


		for (int i = 0; i < 6; i++) {
			specular_spiders[i].move(delta);
			specular_spiders[i].update_leg_rotation(delta);
		}
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