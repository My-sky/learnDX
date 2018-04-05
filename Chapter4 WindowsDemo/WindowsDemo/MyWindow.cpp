#include"MyWindow.h"
#include "BlankDemo.h"

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//��̬�ص�����
static MyWindow *applicationHandle;//���һ����̬ʵ��

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
	m_hinstance = GetModuleHandle(NULL);//�õ�����ʵ�����

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
	//ע�ᴰ����
	if (!RegisterClassEx(&wnd))
	{
		MessageBox(NULL, L"ע�ᴰ��ʧ�ܣ�", L"Hello Win", 0);
		return false;
	}

	//��������
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_name, m_name, WS_OVERLAPPEDWINDOW, 300, 200, width, height, NULL, NULL, m_hinstance, NULL);

	//��ʾ����������Ϊ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//�������
	SetCursor(false);
	return true;
}

void MyWindow::Run()
{
	MSG msg;

	//��ʼ����Ϣ�ṹ
	ZeroMemory(&msg, sizeof(MSG));

	bool isRunning = true;//�����Ƿ��˳���Ϣѭ��
	Dx11DemoBase *dxRender = new BlankDemo();
	if (FAILED(dxRender->InitDevice(m_hwnd)))
	{
		
		return;
	}

	while (isRunning)
	{
		//����Windows��Ϣ
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
		if (wparam == VK_ESCAPE)//�û������˳���
			isPushEsc = true;
		return 0;

		//������Ϣʹ��Windowsȱʡ����
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

		//������Ϣ����MessageHandler����
	default:
		return applicationHandle->MessageHandler(hwnd, message, wparam, lparam);
	}
}