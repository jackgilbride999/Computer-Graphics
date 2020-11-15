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

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define SPIDER_MESH_NAME "spider.dae"
#define LEG_MESH_NAME "leg.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
} ModelData;
#pragma endregion SimpleTypes

using namespace std;
GLuint shaderProgramID;

ModelData spider_mesh_data;
ModelData leg_mesh_data;
unsigned int mesh_vao = 0;
int width = 800;
int height = 600;

unsigned int spider_vao = 0;
unsigned int leg_vao = 0;


GLuint loc1, loc2, loc3;

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

#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name, 
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	); 

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh(ModelData mesh_data, unsigned int vao) {

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	//mesh_data = load_mesh(MESH_NAME);																					// load the spider mesh
	//mesh_data = load_mesh(mesh_name);																					// load the spider mesh
	
																														//unsigned int vao = 0;																								// a pointer to the vao
	glBindVertexArray(vao);																								// bind the vao

	unsigned int vp_vbo = 0;																							// a pointer to vertex position vbo
	glGenBuffers(1, &vp_vbo);																							// generate buffer for the vertex position vbo
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);																				// bind a buffer to the vertex position vbo
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);		// load data to the vertex position vbo

	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");														// a pointer to vertex position in the shader
	glEnableVertexAttribArray(loc1);																					// point at the vertex position in the shader
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);																				// bind the vertex position vbo to it
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);														// store vertex position location in the vao
	
	unsigned int vn_vbo = 0;																							// a pointer to the vertex normal vbo
	glGenBuffers(1, &vn_vbo);																							// generate buffer for the vertex normal vbo
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);																				// bind a buffer to the vertex normal vbo
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);		// load data to the vertex normal vbo

	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");														// a pointer to vertex normal in the shader
	glEnableVertexAttribArray(loc2);																					// point at the vertex normal in the shader
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);																				// bind the vertex normal vbo to it
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);														// store vertex normal location in the vao

	unsigned int vt_vbo = 0;
	glGenBuffers (1, &vt_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	glBufferData (GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof (vec3), &mesh_data.mTextureCoords[0], GL_STATIC_DRAW);
	
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");														// a pointer to the vertex texture in the shader
	glEnableVertexAttribArray (loc3);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}
#pragma endregion VBO_FUNCTIONS


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
	glBindVertexArray(spider_vao);
	glDrawArrays(GL_TRIANGLES, 0, spider_mesh_data.mPointCount);

	glBindVertexArray(leg_vao);

	// Set up the child matrix
	mat4 modelChild = identity_mat4();
	modelChild = rotate_z_deg(modelChild, 180);
	modelChild = rotate_y_deg(modelChild, rotate_z);
	modelChild = translate(modelChild, vec3(0.0f, 1.9f, 0.0f));
	// Apply the root matrix to the child matrix
	modelChild = model * modelChild;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild.m);
	glDrawArrays(GL_TRIANGLES, 0, leg_mesh_data.mPointCount);
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
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array
	spider_mesh_data = load_mesh(SPIDER_MESH_NAME);
	glGenVertexArrays(1, &spider_vao);
	generateObjectBufferMesh(spider_mesh_data, spider_vao);


	leg_mesh_data = load_mesh(LEG_MESH_NAME);
	glGenVertexArrays(1, &leg_vao);
	generateObjectBufferMesh(leg_mesh_data, leg_vao);

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
