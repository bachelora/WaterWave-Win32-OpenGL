#include "OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern void OutPutDebugStringF(const char* format, ...) {
    va_list vlArgs;
    char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);//GPTR:����̶��ڴ沢��ʼ��Ϊ0
    va_start(vlArgs, format);
    _vsnprintf_s(strBuffer, 4096, 4096 - 1, format, vlArgs);//��дʹ��ָ������б�ĸ�ʽ�����
    va_end(vlArgs);
    OutputDebugStringA(strBuffer);
    GlobalFree(strBuffer);//�ͷ��ڴ�
}

OpenGL::OpenGL():hRC(nullptr),_angle(0),_vbo(0) {
}

void OpenGL:: EnableOpenGL(HWND hWnd)
{

    PIXELFORMATDESCRIPTOR pfd;
    int format;

    // get the device context (DC)
    HDC hdc = GetDC(hWnd);

    // set the pixel format for the DC
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, format, &pfd);

    // create and enable the render context (RC)
    hRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hRC);

    shader1 = ShaderProgram("shader/vertexShader.vs", "shader/fragraShader.fs");

    _displayList = dispalyList();
   
    _config = glm::vec4(100,100,50,50);//width height width's piece height's piece
    _vbo = generateVBO();
    _vao = generateVAO();

   
    _texture1 = generateTextureId("res/456.jpg");    
    _texture2 = generateTextureId("res/container.jpg");
}


/*
* 1.glVertex
������ԭʼ�����ö��㷽������glBegin��glEnd֮��ʹ�á�ÿ�λ��Ƶ�ʱ��Ҫһ������һ������Ĵ��䡣OpenGL3.0�Ѿ������˷�����ÿ��glVertex��GPU����һ��ͨ�ţ�ʮ�ֵ�Ч��
*/
void OpenGL::draw1111() {
    glBegin(GL_TRIANGLES);
       glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 0.50f);
       glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.87f, -0.5f);
       glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(-0.87f, -0.5f);
     glEnd();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/*
* 2.��ʾ�б�(glCallList)
����ÿ��glVertex���ö���GPU����һ��ͨ�ţ���ʾ�б��ǻ���ǰ�ռ������еĶ��㣬һ���Եķ��͸�GPU��ȱ�����ڻ���֮ǰ��Ҫ��Ҫ����GPU�Ķ���׼���ã�����Ͳ����޸��ˡ�
  Display List
ʹ��glVertex�ķ�ʽ�������ݣ����������ͣ���ô����Ч�ʻ�Ѹ�ٽ��͡�����ͼ������򵥣�ÿһ�λ��ƴ�������ݣ��������������ȫ��ͬ�ġ����ܲ�����ÿһ������ֻ��һ���أ�

Display List����ʾ�б�Ӧ�˶�����

��glNewList��glEndList���ѷ�����֮�䣬�����㴫����̰�������������ζ�����ռ��ö��㣬ͳһ�����GPU����������GPU�ϣ��������ظ����Ƶ�ʱ�����ֱ�Ӵ�GPU��ȡ���ݣ��������´��䣬�Դ���Ч�ʵ������Ǽ���ġ�

��ʾ�б�ľ�����Ҳ�����ԣ�û���ڻ���ʱ�޸Ķ������ݣ����Ҫ�޸Ķ������ݣ�ֻ����CPU���޸������´���һ�ݡ���������£����������������ÿ֡��Ҫ�仯����ʾ�б����ȫ�˻����� glVertex ģʽ��
*/
GLuint OpenGL::dispalyList() {
    // ֻ�ڳ�ʼ����ʱ��������
    GLuint listName = glGenLists(1);
    glNewList(listName, GL_COMPILE);
      glBegin(GL_TRIANGLES);
       glVertex2f(0.0, 0.0);
       glVertex2f(1.0, 0.0);
       glVertex2f(0.0, 1.0);
      glEnd();
    glEndList();
       
    return listName;
   // glCallList(listName);// ����ʱ���ã����������ݣ�
}


/*
* 3.Vertex Array
��������Ķ���仯����VA���������飩���뵽�˹淶���ÿһ�λ��ƣ����ռ��Ķ���ͨ��һ��API���ô����GPU���׳ƴ�����ݴ��䡣

VA��������ʾ�б��������ڣ����ռ��Ķ��㱣����CPU�ˣ�ÿ�λ��ƶ���Ҫ���´�һ�����ݣ����Ի����ٶ�����������ʾ�б�ע�⣺����������GL���õģ�������ֻ��ѡ���������
*/
void OpenGL::draw2222() {
    // ÿ�λ��ƶ��� vertices ����һ��,�����ǵ���һ��API������������
    GLfloat vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f
    };
   
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


/*
* 4.VBO (Vertex Buffer Object)
VBO����֮ǰ����OpenGL�Ż�����߶������Ч�ʵİ취һ������֣�

��ʾ�б��ѳ���Ļ��ƴ������һ����ʾ�б��У�ͨ���ڳ�ʼ���׶���ɣ��������ݻ�����Ҫһ��������ģ�����Ⱦʱֱ��ʹ�������ʾ�б��Ż��㣺�������ݴ������
�������飺�Ѷ����Լ������������ݴ���ɵ������飬��Ⱦʱֱ�Ӵ�������顣�Ż��㣺�����˺������ô���������glVertex��

VBO��Ŀ������������Ƽ�ã��뽫��ʾ�б�����ԣ�����ʱ���������ݣ��죩�Ͷ�����������ԣ����ݴ�����䣬�޸������������

��Ȼ����Ч����ǿ���⣬Ч�ʽ�������֮�䣬ӵ�����õ������޸ĵ��ԡ�����Ⱦ�׶Σ����ǿ��԰Ѹ�֡������ˮ�ߵĶ�������ӳ���client���޸ģ�vertex mapping����Ȼ�����ύ����ˮ�ߣ�vertex unmapping������ζ�Ŷ�������ֻ��VBO����һ�ݣ����߿����� glBufferData(ȫ������)\glBufferSubData(��������) �ύ�����˵Ķ������ݣ���ζ�Ŷ���������client�˺�VBO�ﶼ��һ�ݡ�

VBO��������һ������buffer�����棩����Ӧ��client�˵�ĳ�����ݣ������ݴ����VBO֮��client�˵������ǿ���ɾ���ġ�ϵͳ������û����õ� target �� usage ������VBO���ʺϵĴ��λ�ã�ϵͳ�ڴ�/AGP/�Դ棩����Ȼ��GL�淶��һ���£��Կ����̵�����ʵ��������һ�����ˡ�

�ڳ�ʼ���׶Σ�VBO�ǲ�֪�������洢����ʲô���ݣ���������Ⱦ�׶Σ���ȷ˵�� glVertexAttribPointer ��������ȷ�������������ͣ�����λ�á�float���͡���ƫ����0����ʼ�ɼ����ݡ�2��float��һ���ɼ������ȵȣ������������ƣ�glDrawArray/glDrawElement����ʱ��Ŵ�VBO���ȡ��Ҫ�����ݽ�����Ⱦ��ˮ�ߡ�
*/
GLuint OpenGL::generateVBO() {
    // ��ʼ��
 
    GLfloat vertices[] = {
        -50.0f ,-50.0f ,  0,0,
        -50.0f + _config.x/_config.z , -50.0f ,  1.0/_config.z,0,
        -50.0f + _config.x /_config.z , -50.0f+_config.y/ _config.w ,  1.0/_config.z,1.0/ _config.w,
        -50.0f , -50.0f + _config.y / _config.w ,  0,1.0/ _config.w,
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);//���
    return vbo;

    //// ����ʱ����
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,2*sizeof(GLfloat), (void*)0);
    //glDrawArray(GL_TRIANGLES, 0, 3);
}


/*
* 5.VAO
VAO��ȫ����Vertex Array Object�����ȣ�������Buffer-Object�����Բ������洢���ݣ���Σ�����ԡ����㡰���ԣ�Ҳ����˵����������Ļ��ơ�ϢϢ��أ����൱�ڡ���VBOϢϢ��ء�������ʾ������VA����Ϻ�׹�ϵ��û�еģ����Ȼ���ȷ������ᣬ����������������ʱҲ����˵�˵����

�������������Ķ�λ��state-object��״̬���󣬼�¼�洢״̬��Ϣ����������������buffer-object����������������Ȼ�Ǽ�¼�������Ϣ��Ϊʲô����vertex attribute object���أ�����˵��Щ����������û���濴�������VAO��¼����һ�λ���������Ҫ����Ϣ�������������������-glBindBuffer(GL_ARRAY_BUFFER)���������ݵĸ�ʽ��������-glVertexAttribPointer��������λ�õ��������������λ�õ����ݵĸ�ʽ��������/������������������������������ݵĸ�ʽ��������....�Ӻ��������������ٸ�VBO��VBO���ж��������ݣ���˳��һ����ǣ������״̬��������Щ���Թ�����shader-attribute��location�����ã�glEnableVertexAttribArray������Щ�������Զ�Ӧ�Ķ����������ݵ�λ�ã�glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)�������ָ���˵Ļ�����

struct VertexAttribute  
{  
    bool bIsEnabled = GL_FALSE;  
    int iSize = 4; //This is the number of elements in this attribute, 1-4.  
    unsigned int iStride = 0;  
    VertexAttribType eType = GL_FLOAT;  
    bool bIsNormalized = GL_FALSE;  
    bool bIsIntegral = GL_FALSE;  
    void * pBufferObjectOffset = 0;  
    BufferObject * pBufferObj = 0;  
};  
   
struct VertexArrayObject  
{  
    BufferObject *pElementArrayBufferObject = NULL;  
    VertexAttribute attributes[GL_MAX_VERTEX_ATTRIB];  
}  
���VertexArrayObject �Ͱ�����һ��Index-VBO������û�У���������õ���glDrawArray������һЩVertexAttribute�����߰����������Եĸ�ʽ��λ�ú�һ����������״̬����Щ����Ӧ���������۵��Ǽ���������ע��glVertexAttribPointer��glVertexAttribIPointer��ѡ�����bool bIsIntegral�������Ƿ����Ͳ��ɹ淶��������ô���������ǿ���֪��VAO���÷��ˣ�
*/
GLuint OpenGL::generateVAO() {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo = generateVBO();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);//�������ݸ�ʽ��������Դ�뵱ǰ�󶨵�GL_ARRAY_BUFFER


    int width, height, nrChannels;
   // stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);//���Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
    stbi_image_free(data);

    GLfloat vertices[] = {
        0.0f, 1.0f,
        1.0f, 1.20f,
        1.0f, 10.0f,
        1.00f, 0.0f,

        10.0f, 10.05f,
        10.0f, 10.08f,
        10.0f, 10.0f,
        0.06f, 10.0f,
    };
    GLuint vbo_texCoord;
    glGenBuffers(1, &vbo_texCoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texCoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    
    unsigned char indices[] = {
        0,1,3,
        1,2,3,

        4,5,7,
        5,6,7,

        3,2,7,
        2,6,7,

        0,1,4,
        1,5,4,

        0,3,4,
        3,7,4,

        1,2,5,
        5,2,6
    };

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    //
    GLfloat colors[] = {
        0.0f, 0.0f,0.20,
        0.0f, 1.0f,0.0,
        1.0f, 0.0f,0.0,
        1.0f, 1.0f,1.0,

        0.0f, 0.40f,0.0,
        0.0f, 1.0f,0.30,
        1.0f, 0.10f,0.20,
        1.0f, 1.0f,1.0
    };

    GLuint vbo_Color;
    glGenBuffers(1, &vbo_Color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

   

    glBindVertexArray(0);
   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   // glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;

    ///ʹ��ʱ��ֱ�Ӽ���
    ///glBindVertexArray(_vao);
    ///glDrawArrays(GL_TRIANGLES, 0, 3);
}


GLuint OpenGL::generateTextureId(string name) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);//���Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}


void OpenGL::paint() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    //glfro
   // glFrontFace(GL_CW);
   // glEnable(GL_CULL_FACE);
  //  glFrontFace(GL_CW);

    glDisable(GL_CULL_FACE);
   
    glEnable(GL_DEPTH_TEST);
   
    glm::mat4 trans(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(50.0f), 2.f, 100.0f, -100.0f);  // glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 100.0f, -100.0f);//
        //glm::perspective(glm::radians(45.0f), 3.0f / 3, 0.001f, 100.0f);
  // _angle += 2;
   // glm::vec4 point(-30,-20,0,1);
  //  point = projection * point;

    glm::mat4 view = glm::lookAt(glm::vec3(0, 0.0, 200.1), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //view = glm::translate(view, glm::vec3(0.0f,0.0f, 0.990f));
    //if (_angle >= 360){
    //    _angle = 0;
   // }
   // trans = glm::scale(trans, glm::vec3(0.8, 0.8, 0.8));
    trans = glm::rotate(trans, glm::radians(_angle), glm::vec3(10.0,0,0.0));
   // trans  = glm::translate(trans, glm::vec3(0.0f, 0.50f, 0.0f));
   
    glm::mat4 mvp = projection * view * trans;
    glm::mat4 inverse = glm::inverse(mvp);//�����
   
    glm::vec4 test444 = mvp * glm::vec4(-50, -50, 0, 1);

   // gluUnProject;
   // glm::vec4 test444 = inverse * test55;

   // NSLog("===x=%lf,y=%lf,z=%lf,w=%lf\n",test444.x,test444.y,test444.z,test444.w);

    glm::vec4 worldCoodinate = inverse * glm::vec4(_centerP.x,_centerP.y,-1,1);
    glm::vec2 world2 = glm::vec2(worldCoodinate.x, worldCoodinate.y);
   // NSLog("world.x=%lf,world.y=%lf\n", world2.x, world2.y);
    shader1.use();

    glm::vec2 length = glm::vec2(_angle,-5);
   // NSLog("--%lf",_angle);
    shader1.setUniformVec2fv("length", length);//center
    shader1.setUniformVec2fv("center", world2);//
    shader1.setUniformVec3fv("direction", direction);
    shader1.setUniformVec2fv("point",point);

    shader1.setUniformVec4f("m_n",_config.x/_config.z,_config.y/_config.w,_config.z,_config.w);
    shader1.setUniformMatrix4fv("model",trans);
    shader1.setUniformMatrix4fv("projection",projection);
    shader1.setUniformMatrix4fv("view",view);

    glUniform1i(glGetUniformLocation(shader1.programId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader1.programId, "texture2"), 1);
   // draw1111();
   // glCallList(_displayList);
   // draw2222();
   // glDrawArrays(GL_TRIANGLES, 0, 3);
    //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
  
    // ����
     //// ����ʱ����
    
    //glDrawArray(GL_TRIANGLES, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER,_vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE,4*sizeof(GLfloat),(void*)0);
    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture2);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, _config.w * _config.z);
   // glBindVertexArray(_vao);
   // glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
   // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

    /*glPushMatrix();
    glRotatef(2.05, 0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 0.50f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.5f, -0.50f);
    glEnd();
    glPopMatrix();*/
}