#include"MyWindow.h"
#include "BlankDemo.h"

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//静态回调函数
static MyWindow *applicationHandle;//类的一个静态实例

MyWindow::MyWindow()
{
	m_hinstance = NULL;
	m_hwnd = NULL;
	m_name = L"Hello Win";
	isPushEsc = false;
}
MyWindow::~MyWindow()
{

}

HWND MyWindow::GetHandle()
{
	return m_hwnd;
}

bool MyWindow::Create(int &width, int &height)
{
	applicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);//得到程序实例句柄

	WNDCLASSEX wnd;
	wnd.cbClsExtra = 0;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd.hIconSm = wnd.hIcon;
	wnd.hInstance = m_hinstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = m_name;
	wnd.lpszMenuName = m_name;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	//注册窗口类
	if (!RegisterClassEx(&wnd))
	{
		MessageBox(NULL, L"注册窗口失败！", L"Hello Win", 0);
		return false;
	}

	//创建窗口
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_name, m_name, WS_OVERLAPPEDWINDOW, 300, 200, width, height, NULL, NULL, m_hinstance, NULL);

	//显示窗口设置其为焦点
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//隐藏鼠标
	SetCursor(false);
	return true;
}

void MyWindow::Run()
{
	MSG msg;

	//初始化消息结构
	ZeroMemory(&msg, sizeof(MSG));

	bool isRunning = true;//控制是否退出消息循环
	Dx11DemoBase *dxRender = new BlankDemo();
	if (FAILED(dxRender->InitDevice(m_hwnd)))
	{
		
		return;
	}

	while (isRunning)
	{
		//处理Windows消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(msg.message == WM_QUIT)
		{
			isRunning = false;
		}
		else
		{
			isRunning = !isPushEsc;
			dxRender->Update(0.0);
			dxRender->Render();
		}
	}
	dxRender->CleanupDevice();
}

LRESULT CALLBACK MyWindow::MessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)//用户按下退出键
			isPushEsc = true;
		return 0;

		//其它消息使用Windows缺省处理
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		//其他消息发送MessageHandler处理
	default:
		return applicationHandle->MessageHandler(hwnd, message, wparam, lparam);
	}
}