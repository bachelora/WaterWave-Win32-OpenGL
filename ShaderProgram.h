#include<string>
#include"framework.h"
#include<windows.h>

using namespace std;

#pragma comment(lib,"GL/lib/glew32s.lib")
#pragma once


class ShaderProgram
{
private:
	string _readFileFrom(string path);
	
	static unsigned int _compile(string&code,GLenum type); 
public:
	ShaderProgram();

	inline void setUniformVec2fv(const char* name, glm::vec2& v) {
		unsigned int transformLoc = glGetUniformLocation(programId, name);
		glUniform2fv(transformLoc, 1, glm::value_ptr(v));
	}

	inline void setUniformVec3fv(const char* name,glm::vec3 &v) {
		unsigned int transformLoc = glGetUniformLocation(programId, name);
		glUniform3fv(transformLoc,1,glm::value_ptr(v));
	}

	inline void setUniformVec4fv(const char* name,glm::vec4 &v) {
		unsigned int transformLoc = glGetUniformLocation(programId, name);
		glUniform4fv(transformLoc,1,glm::value_ptr(v));
	}

	inline void setUniformVec4f(const char* name, GLfloat x,GLfloat y,GLfloat z,GLfloat w) {
		unsigned int transformLoc = glGetUniformLocation(programId, name);
		glUniform4f(transformLoc,x,y,z,w);
	}

	inline void setUniformMatrix4fv(const char* name,const glm::mat4& matrix) {
		unsigned int transformLoc = glGetUniformLocation(programId,name);
	    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	}

    inline void use() {
		if (programId > 0){
			glUseProgram(programId);
		}
	}

	ShaderProgram(string vertexShaderPath,string fragmentShaderPath);

	unsigned int programId;

};

