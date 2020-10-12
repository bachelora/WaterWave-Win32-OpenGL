// LearningOpenGL.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "LearningOpenGL.h"
#include "OpenGL.h"
#include<iostream>

#define _USE_MATH_DEFINES
#include<math.h>
using namespace std;

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
OpenGL openGl;
bool isPressing = false;
UINT timerID = 0;
LPARAM startXy;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LEARNINGOPENGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEARNINGOPENGL));

    MSG msg;
   
    
    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEARNINGOPENGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LEARNINGOPENGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   openGl.EnableOpenGL(hWnd);
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
           
            openGl.paint();
            SwapBuffers(hdc);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_RBUTTONDOWN:
    {
        int w = LOWORD(lParam);
        int h = HIWORD(lParam);
        
        RECT rect;
        GetClientRect(hWnd,&rect);
        LONG width = rect.right - rect.left;
        LONG height = rect.bottom - rect.top;
        openGl._centerP = glm::vec2(((double)w-width/2)/ (width/2),-((double)h-height/2)/ (height/2));//标准化坐标



        NSLog("Right down--x:%f--y:%f\n", openGl._centerP.x, openGl._centerP.y);

        if (timerID) {
            KillTimer(hWnd, timerID);
            timerID = 0;
        }
        else {
            openGl._angle = 0.8;
            timerID = SetTimer(hWnd, 10, 50, TimeProc);
        }
    }
        
        
        break;
    case WM_MOUSEWHEEL:
    {
        SHORT aa = HIWORD(wParam);
        if (aa > 0){
            openGl.direction = glm::vec3(openGl.direction.x, openGl.direction.y, openGl.direction.z+3);
        }else {
            openGl.direction = glm::vec3(openGl.direction.x, openGl.direction.y, openGl.direction.z-3);
        }
        NSLog("WM_MOUSEWHEEL--LOWORD-wParam=%d  %d  %d  %d\n", LOWORD(wParam), aa, LOWORD(lParam), HIWORD(lParam));
    }
    PostMessage(hWnd, WM_PAINT, 0, 0);
        break;
    case WM_LBUTTONDOWN:
        isPressing = true;
        startXy = lParam;
        NSLog("left down");
        break;
    case WM_LBUTTONUP:
        isPressing = false;
        NSLog("left up");
        break;
    case WM_MOUSEMOVE:

    if (isPressing){
        
        WORD _x = LOWORD(startXy);
        WORD _y = HIWORD(startXy);
        
        WORD x = LOWORD(lParam);
        WORD y = HIWORD(lParam);

        glm::vec2 direction(x-_x,_y-y);
        openGl.direction = glm::vec3(1,0,openGl._config.x/M_PI/2);
        float leftPos = -openGl._config.x / 2;
        float X = glm::clamp(leftPos + glm::length(direction)/5,leftPos, -leftPos);
        openGl.point = glm::vec2(X,0);
        ;

        direction;
        if (direction.x> 0 && direction.y<=0){
            
        }else if (direction.x >= 0 && direction.y > 0) {
           // openGl.point = glm::vec2(-openGl._config.x / 2, -openGl._config.y / 2) + 0.50f * direction;
        }else if (direction.x <= 0 && direction.y <= 0) {
           // openGl.point = glm::vec2(openGl._config.x / 2, openGl._config.y / 2) + 0.50f * direction;
        }else {
           // openGl.point = glm::vec2(openGl._config.x / 2, -openGl._config.y / 2) + 0.50f * direction;
        }

       

        PostMessage(hWnd, WM_PAINT, 0, 0);
        
    } 
        break;
    case WM_SIZE:
    {
        int w = LOWORD(lParam);
        int h = HIWORD(lParam);
        NSLog("changeSize--width:%d--height:%d\n", w, h);
        glViewport(0, 0, w, h);
       
    }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//定时器回调函数
void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime) {
    static float aaa = 0.01;
  //  aaa += 0.0001;
    openGl._angle -= aaa;
    if (openGl._angle <= 0)
    {
        openGl._angle = 0.0f;
        KillTimer(hwnd, timerID);
        timerID = 0;
    }
    PostMessage(hwnd, WM_PAINT, 0, 0);
   // NSLog("--TimeProc---");
}
