#ifndef D3DAPP_H
#define D3DAPP_H

#include <windows.h>
#include <string>
#include "D3DUtil.h"
#include "GameTimer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst() const;//返回应用实例句柄的拷贝
	HWND MainWnd() const;//返回主窗口句柄的拷贝
	float AspectRatio() const;//返回后向缓冲的宽高比

	int Run();//执行应用信息循环

	virtual bool Init();//初始化（分配资源，初始化物件，设置光源），调用InitMainWindow 和InitDirect3D
	virtual void OnResize();//当MsgProc收到WM_SIZE信息时调用这个方法，当窗口大小改变时，buffer的大小，视图的大小，投影矩阵等都需要进行修正，所以这个需要用户自己定义
	virtual void UpdateScene(float dt) = 0;//每帧都会调用，用于每隔dt时间更新3D应用
	virtual void DrawScene();//每帧调用，发出渲染命令，实际绘制当前帧到后向缓冲，绘制完成后调用哪个present显示到屏幕上
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);//为主应用窗口执行窗口处理函数，需要添加消息时可以重载这个函数

	//鼠标事件
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

protected:
	bool InitMainWindow();//初始化窗口
	bool InitDirect3D();//初始化D3D
	//计算FPS
	void CalculateFrameStats();

protected:
	HINSTANCE m_hAppInst;                //实例句柄
	HWND m_hMainWnd;                     //主窗口句柄
	bool m_isAppPaused;
	bool m_isMinimized;
	bool m_isMaximized;
	bool m_isResizing;
	UINT m_4xMsaaQuality;                //4重采样质量

	GameTimer m_timer;                   //计时器
	D3D_DRIVER_TYPE m_d3dDriverType;     //驱动类型
	ID3D11Device *m_pD3dDevice;          //设备
	ID3D11DeviceContext *m_pImmediateContext;  //设备上下文
	IDXGISwapChain  *m_pSwapChain;       //交换链
	ID3D11Texture2D *m_pDepthStencilBuffer;   //深度模板缓冲
	ID3D11RenderTargetView *m_pRenderTargetView;  //渲染目标视图
	ID3D11DepthStencilView *m_pDepthStencilView;  //深度模板缓冲视图
	D3D11_VIEWPORT m_screenViewPort;      //视口

	std::wstring m_mainWndCaption;      //窗口标题
	int m_clientWidth;
	int m_clientHeight;
	bool m_isEnable4xMsaa;              //是否支持4重采样
};
#endif
