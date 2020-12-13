#pragma once
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

// Project includes
#include "maths_funcs.h"

enum shader_types { AMBIENT, DIFFUSE, SPECULAR, TEXTURE };

class Shaders
{
public :
	GLuint shaderProgramID;
	unsigned int shaderType;
	// locations of uniforms in all shaders:
	GLuint uniform_location_model;
	GLuint uniform_location_view;
	GLuint uniform_location_proj;
	// locations of uniforms in diffuse & specular shaders:
	GLuint uniform_location_light_position;
	GLuint uniform_location_object_location;
	// locations of uniforms in specular shaders:
	GLuint uniform_location_view_pos;
	GLuint uniform_location_specular_coef;

	Shaders(unsigned int shaderType);

private:
	static char* readShaderSource(const char* shaderFile) {
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

	static GLuint CompileShaders(std::string vertexShaderName, std::string fragmentShaderName)
	{
		//Start the process of setting up our shaders by creating a program ID
		//Note: we will link all the shaders together into this ID
		fprintf(stderr, "compileshaders called");
		GLuint shaderProgramID = glCreateProgram();
		if (shaderProgramID == 0) {
			std::cerr << "Error creating shader program..." << std::endl;
			std::cerr << "Press enter/return to exit..." << std::endl;
			std::cin.get();
			exit(1);
		}

		// Create two shader objects, one for the vertex, and one for the fragment shader
		AddShader(shaderProgramID, vertexShaderName.c_str(), GL_VERTEX_SHADER);
		AddShader(shaderProgramID, fragmentShaderName.c_str(), GL_FRAGMENT_SHADER);

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
};

