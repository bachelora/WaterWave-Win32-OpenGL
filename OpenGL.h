#pragma once
#include "framework.h"
#include"ShaderProgram.h"
#pragma comment(lib,"opengl32.lib")





class OpenGL
{
private:
	

	GLuint _texture1;
	GLuint _texture2;

	GLuint _vao;
	GLuint _vbo;
	GLuint _displayList;
	
	ShaderProgram shader1;
	HGLRC hRC;

	//unsigned int  VertexBufferObject();

	void draw1111();
	void draw2222();
	GLuint generateVBO();
	GLuint dispalyList();
	
	GLuint generateTextureId(string name);

	GLuint generateVAO();
	
public:
	glm::vec2 _centerP;
	glm::vec4 _config;
	float _angle;

	glm::vec3 direction;
	glm::vec2 point;

	OpenGL();

	void EnableOpenGL(HWND hWnd);

	

	void paint();
};

