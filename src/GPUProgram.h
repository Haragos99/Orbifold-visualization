#pragma once
#include <GL/glew.h>
#include "algebra.h"
#include <vector>
#include <string>
#include "texture.h"
#include <fstream>
#include <iostream>
#include <filesystem>




class GPUProgram {
	//--------------------------
	unsigned int shaderProgramId = 0;
	unsigned int vertexShader = 0, geometryShader = 0, fragmentShader = 0;
	bool waitError = true;

	void getErrorInfo(unsigned int handle);

	bool checkShader(unsigned int shader, std::string message);

	bool checkLinking(unsigned int program);

	int getLocation(const std::string& name);

public:
	GPUProgram(bool _waitError = true) { shaderProgramId = 0; waitError = _waitError; }

	GPUProgram(const GPUProgram& program) {
		if (program.shaderProgramId > 0) printf("\nError: GPU program is not copied on GPU!!!\n");
	}

	void operator=(const GPUProgram& program) {
		if (program.shaderProgramId > 0) printf("\nError: GPU program is not copied on GPU!!!\n");
	}

	unsigned int getId() { return shaderProgramId; }

	bool create(std::string vertexShaderSource, std::string fragmentShaderSource, const char* const fragmentShaderOutputName, const char* const geometryShaderSource = nullptr);
	

	void Use() { 		// make this program run
		glUseProgram(shaderProgramId);
	}

	void setUniform(int i, const std::string& name);

	void setUniform(float f, const std::string& name);

	void setUniform(const vec2& v, const std::string& name);

	void setUniform(const vec3& v, const std::string& name);

	void setUniform(const vec4& v, const std::string& name);

	void setUniform(const mat4& mat, const std::string& name);

	void setUniform(const Texture& texture, const std::string& samplerName, unsigned int textureUnit = 0);
	std::string readShaderFromFile(const std::string& filePath);

	~GPUProgram() { if (shaderProgramId > 0) glDeleteProgram(shaderProgramId); }
};
