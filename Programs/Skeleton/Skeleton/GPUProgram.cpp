#include "GPUProgram.h"


void GPUProgram::getErrorInfo(unsigned int handle) { // shader error report
	int logLen, written;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		std::string log(logLen, '\0');
		glGetShaderInfoLog(handle, logLen, &written, &log[0]);
		printf("Shader log:\n%s", log.c_str());
		if (waitError) getchar();
	}
}

bool GPUProgram::checkShader(unsigned int shader, std::string message) { // check if shader could be compiled
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) {
		printf("%s!\n", message.c_str());
		getErrorInfo(shader);
		return false;
	}
	return true;
}


bool GPUProgram::checkLinking(unsigned int program) { 	// check if shader could be linked
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) {
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
		return false;
	}
	return true;
}



int GPUProgram::getLocation(const std::string& name) {	// get the address of a GPU uniform variable
	int location = glGetUniformLocation(shaderProgramId, name.c_str());
	if (location < 0) printf("uniform %s cannot be set\n", name.c_str());
	return location;
}


bool GPUProgram::create(std::string vertexShaderSource,
	std::string fragmentShaderSource, const char* const fragmentShaderOutputName,
	const char* const geometryShaderSource)
{
	std::filesystem::path curent = std::filesystem::current_path();
	std::string path = curent.string()+"\\";
	

	std::string vs = readShaderFromFile(path + vertexShaderSource);
	std::string fs = readShaderFromFile(path + fragmentShaderSource);

	const char* const vertexShader_ = vs.c_str();
	const char* const fragmentShader_ = fs.c_str();
	// Create vertex shader from string
	if (vertexShader == 0) vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (!vertexShader) {
		printf("Error in vertex shader creation\n");
		exit(1);
	}
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexShader_, NULL);
	glCompileShader(vertexShader);
	if (!checkShader(vertexShader, "Vertex shader error")) return false;

	// Create geometry shader from string if given
	if (geometryShaderSource != nullptr) {
		if (geometryShader == 0) geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		if (!geometryShader) {
			printf("Error in geometry shader creation\n");
			exit(1);
		}
		glShaderSource(geometryShader, 1, (const GLchar**)&geometryShaderSource, NULL);
		glCompileShader(geometryShader);
		if (!checkShader(geometryShader, "Geometry shader error")) return false;
	}

	// Create fragment shader from string
	if (fragmentShader == 0) fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragmentShader) {
		printf("Error in fragment shader creation\n");
		exit(1);
	}

	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShader_, NULL);
	glCompileShader(fragmentShader);
	if (!checkShader(fragmentShader, "Fragment shader error")) return false;

	shaderProgramId = glCreateProgram();
	if (!shaderProgramId) {
		printf("Error in shader program creation\n");
		exit(1);
	}
	glAttachShader(shaderProgramId, vertexShader);
	glAttachShader(shaderProgramId, fragmentShader);
	if (geometryShader > 0) glAttachShader(shaderProgramId, geometryShader);

	// Connect the fragmentColor to the frame buffer memory
	glBindFragDataLocation(shaderProgramId, 0, fragmentShaderOutputName);	// this output goes to the frame buffer memory

	// program packaging
	glLinkProgram(shaderProgramId);
	if (!checkLinking(shaderProgramId)) return false;

	// make this program run
	glUseProgram(shaderProgramId);
	return true;
}


void GPUProgram::setUniform(int i, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1i(location, i);
}

void GPUProgram::setUniform(float f, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1f(location, f);
}

void GPUProgram::setUniform(const vec2& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform2fv(location, 1, &v.x);
}

void GPUProgram::setUniform(const vec3& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform3fv(location, 1, &v.x);
}

void GPUProgram::setUniform(const vec4& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform4fv(location, 1, &v.x);
}

void GPUProgram::setUniform(const mat4& mat, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, mat);
}

void GPUProgram::setUniform(const Texture& texture, const std::string& samplerName, unsigned int textureUnit) {
	int location = getLocation(samplerName);
	if (location >= 0) {
		glUniform1i(location, textureUnit);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture.textureId);
	}
}

std::string GPUProgram::readShaderFromFile(const std::string& filePath) {
	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return "";
	}

	std::string shaderSource;
	std::string line;

	while (std::getline(file, line)) {
		shaderSource += line + "\n";
	}

	file.close();
	return shaderSource;
}
