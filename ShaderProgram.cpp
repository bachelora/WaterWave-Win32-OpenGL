#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>

string ShaderProgram::_readFileFrom(string path) {
    string code;
    std::ifstream shaderFile;
   
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   
    try{
        shaderFile.open(path);
        stringstream stream;
       
        stream << shaderFile.rdbuf();
        shaderFile.close();

        code = stream.str();
    }
    catch (std::ifstream::failure e){
       
    }
    return code;
}

ShaderProgram::ShaderProgram():programId(0) {
}



ShaderProgram::ShaderProgram(string vPath, string fPath):programId(0) {

    GLenum err = glewInit();
    if (GLEW_OK != err){
        NSLog("glewInit failed:%d",err);
        return;
    }

    vPath = _readFileFrom(vPath);
    fPath = _readFileFrom(fPath);
    if (vPath.length() <= 0 || fPath.length()<=0){
        return;
    }
    
    unsigned int ver = _compile(vPath,GL_VERTEX_SHADER);
    if (ver == 0){
        return;
    }
  
    unsigned int frag = _compile(fPath, GL_FRAGMENT_SHADER);
    if (frag == 0) {
        return;
    }

    programId = glCreateProgram();

    glAttachShader(programId, ver);
    glAttachShader(programId, frag);
    glLinkProgram(programId);//将这些对象链接成一个可执行程序  

    GLint linkStatus;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE){
        GLint logLen;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char* log = (char*)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(programId, logLen, &written, log);//得到日志信息并输出  
            NSLog("file:%s,fuc:%s,line:%ld,message:%s\n", __FILE__, __FUNCTION__,__LINE__,log);
            free(log);
        }
    }

    glDeleteShader(ver);
    glDeleteShader(frag);
}
 unsigned int ShaderProgram::_compile(string& code, GLenum type) {
     unsigned int id = 0;

     switch (type)
     {
     case GL_VERTEX_SHADER:
     case GL_FRAGMENT_SHADER:
         id = glCreateShader(type);
         break;
     default:
         return id;
     }
     
     const GLchar* array[1] = { code.c_str() };
     glShaderSource(id, 1, array, NULL);
     glCompileShader(id);

     int success;
     glGetShaderiv(id, GL_COMPILE_STATUS, &success);
     if (success == GL_FALSE) {
         GLint logLen;
         glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
         if (logLen > 0){
             char* log = (char*)malloc(logLen);
             GLsizei written;
             glGetShaderInfoLog(id, logLen, &written, log);//得到日志信息并输出  
             NSLog("file:%s,fuc:%s,message:%s\n",__FILE__,__FUNCTION__,log);
             free(log);
         }
     }
     return id;
}
 