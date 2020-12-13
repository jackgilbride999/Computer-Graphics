#include "Shaders.h"

Shaders::Shaders(unsigned int shaderType) {
	this->shaderType = shaderType;
	switch (shaderType){
	case AMBIENT:
		printf("Created ambient shader\n");
		this->shaderProgramID = Shaders::CompileShaders("simpleVertexShader.txt", "simpleFragmentShader.txt");
		break;
	case DIFFUSE:
		printf("Created diffuse shader\n");
		this->shaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "diffuseFragmentShader.txt");
		break;
	case SPECULAR:
		printf("Created specular shader\n");
		this->shaderProgramID = Shaders::CompileShaders("advancedVertexShader.txt", "specularFragmentShader.txt");
		break;
	case TEXTURE:
		printf("Created texture shader\n");
		this->shaderProgramID = Shaders::CompileShaders("textureVertexShader.txt", "textureFragmentShader.txt");
		break;
	}
	switch (shaderProgramID) {
	case SPECULAR:
		this->uniform_location_view_pos = glGetUniformLocation(shaderProgramID, "view_pos");
		this->uniform_location_specular_coef = glGetUniformLocation(shaderProgramID, "specular_coef");
	case DIFFUSE:
		this->uniform_location_object_location = glGetUniformLocation(shaderProgramID, "object_location");
		this->uniform_location_light_position = glGetUniformLocation(shaderProgramID, "light_position");
	default:
		this->uniform_location_model = glGetUniformLocation(shaderProgramID, "model");
		this->uniform_location_view = glGetUniformLocation(shaderProgramID, "view");
		this->uniform_location_proj = glGetUniformLocation(shaderProgramID, "proj");
	}
}