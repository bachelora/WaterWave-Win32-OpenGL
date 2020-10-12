#include "OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern void OutPutDebugStringF(const char* format, ...) {
    va_list vlArgs;
    char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);//GPTR:分配固定内存并初始化为0
    va_start(vlArgs, format);
    _vsnprintf_s(strBuffer, 4096, 4096 - 1, format, vlArgs);//编写使用指针参数列表的格式化输出
    va_end(vlArgs);
    OutputDebugStringA(strBuffer);
    GlobalFree(strBuffer);//释放内存
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
　　最原始的设置顶点方法，在glBegin和glEnd之间使用。每次绘制的时候都要一个顶点一个顶点的传输。OpenGL3.0已经废弃此方法。每个glVertex与GPU进行一次通信，十分低效。
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
* 2.显示列表(glCallList)
　　每个glVertex调用都与GPU进行一次通信，显示列表是绘制前收集好所有的顶点，一次性的发送给GPU。缺点是在绘制之前就要把要传给GPU的顶点准备好，传后就不能修改了。
  Display List
使用glVertex的方式传输数据，数据量膨胀，那么传输效率会迅速降低。早期图形需求简单，每一次绘制传输的数据，多数情况下是完全相同的。那能不能让每一个数据只传一次呢？

Display List（显示列表）应运而生。

在glNewList、glEndList（已废弃）之间，将顶点传输过程包裹了起来，意味着它收集好顶点，统一传输给GPU，并保存在GPU上，这样在重复绘制的时候可以直接从GPU端取数据，不再重新传输，对传输效率的提升是极大的。

显示列表的局限性也很明显：没法在绘制时修改顶点数据，如果要修改顶点数据，只有在CPU端修改再重新传输一份。极端情况下，如果场景顶点数据每帧需要变化，显示列表就完全退化成了 glVertex 模式。
*/
GLuint OpenGL::dispalyList() {
    // 只在初始化的时候传输三次
    GLuint listName = glGenLists(1);
    glNewList(listName, GL_COMPILE);
      glBegin(GL_TRIANGLES);
       glVertex2f(0.0, 0.0);
       glVertex2f(1.0, 0.0);
       glVertex2f(0.0, 1.0);
      glEnd();
    glEndList();
       
    return listName;
   // glCallList(listName);// 绘制时调用（不传输数据）
}


/*
* 3.Vertex Array
针对灵活多变的顶点变化需求，VA（顶点数组）加入到了规范里。它每一次绘制，将收集的顶点通过一次API调用传输给GPU，俗称打包数据传输。

VA与上述显示列表区别在于，它收集的顶点保存在CPU端，每次绘制都需要重新传一次数据，所以绘制速度上面慢于显示列表。注意：顶点数组是GL内置的，开发者只能选择启用与否。
*/
void OpenGL::draw2222() {
    // 每次绘制都将 vertices 传输一次,区别是调用一次API传递所有数据
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
VBO出现之前，做OpenGL优化，提高顶点绘制效率的办法一般就两种：

显示列表：把常规的绘制代码放置一个显示列表中（通常在初始化阶段完成，顶点数据还是需要一个个传输的），渲染时直接使用这个显示列表。优化点：减少数据传输次数
顶点数组：把顶点以及顶点属性数据打包成单个数组，渲染时直接传输该数组。优化点：减少了函数调用次数（弃用glVertex）

VBO的目标就是鱼与熊掌兼得，想将显示列表的特性（绘制时不传输数据，快）和顶点数组的特性（数据打包传输，修改灵活）结合起来。

当然最终效果差强人意，效率介于两者之间，拥有良好的数据修改弹性。在渲染阶段，我们可以把该帧到达流水线的顶点数据映射回client端修改（vertex mapping），然后再提交回流水线（vertex unmapping），意味着顶点数据只在VBO里有一份；或者可以用 glBufferData(全部数据)\glBufferSubData(部分数据) 提交更改了的顶点数据，意味着顶点数据在client端和VBO里都有一份。

VBO本质上是一块服务端buffer（缓存），对应着client端的某份数据，在数据传输给VBO之后，client端的数据是可以删除的。系统会根据用户设置的 target 和 usage 来决定VBO最适合的存放位置（系统内存/AGP/显存）。当然，GL规范是一回事，显卡厂商的驱动实现又是另一回事了。

在初始化阶段，VBO是不知道它所存储的是什么数据，而是在渲染阶段（精确说是 glVertexAttribPointer 函数）才确定数据作用类型（顶点位置、float类型、从偏移量0处开始采集数据、2个float算一个采集步长等等）。到真正绘制（glDrawArray/glDrawElement）的时候才从VBO里读取需要的数据进入渲染流水线。
*/
GLuint OpenGL::generateVBO() {
    // 初始化
 
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑
    return vbo;

    //// 绘制时调用
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,2*sizeof(GLfloat), (void*)0);
    //glDrawArray(GL_TRIANGLES, 0, 3);
}


/*
* 5.VAO
VAO的全名是Vertex Array Object，首先，它不是Buffer-Object，所以不用作存储数据；其次，它针对”顶点“而言，也就是说它跟”顶点的绘制“息息相关，这相当于”与VBO息息相关“。（提示，它跟VA真是虾米关系都没有的，嘛，虽然这的确让人误会，我最初见到这个名词时也误会了的说。）

按上所述，它的定位是state-object（状态对象，记录存储状态信息）。这明显区别于buffer-object。如果有人碎碎念”既然是记录顶点的信息，为什么不叫vertex attribute object“呢？我想说这些孩子你们真没认真看文章嘛——VAO记录的是一次绘制中做需要的信息，这包括”数据在哪里-glBindBuffer(GL_ARRAY_BUFFER)“、”数据的格式是怎样的-glVertexAttribPointer“（顶点位置的数据在哪里，顶点位置的数据的格式是怎样的/纹理坐标的数据在哪里，纹理坐标的数据的格式是怎样的....视乎你让它关联多少个VBO、VBO里有多少种数据），顺带一提的是，这里的状态还包括这些属性关联的shader-attribute的location的启用（glEnableVertexAttribArray）、这些顶点属性对应的顶点索引数据的位置（glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)，如果你指定了的话）。

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
这里，VertexArrayObject 就包括了一个Index-VBO（可以没有，例如绘制用的是glDrawArray）还有一些VertexAttribute。后者包括顶点属性的格式和位置和一个启用与否的状态。这些都对应了上述讨论的那几个函数（注意glVertexAttribPointer和glVertexAttribIPointer的选择决定bool bIsIntegral，数据是否整型不可规范化）。那么，现在我们可以知道VAO的用法了：
*/
GLuint OpenGL::generateVAO() {
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo = generateVBO();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);//设置数据格式，数据来源与当前绑定的GL_ARRAY_BUFFER


    int width, height, nrChannels;
   // stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);//这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
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

    ///使用时，直接即可
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
    glGenerateMipmap(GL_TEXTURE_2D);//这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
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
    glm::mat4 inverse = glm::inverse(mvp);//逆矩阵
   
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
  
    // 绘制
     //// 绘制时调用
    
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