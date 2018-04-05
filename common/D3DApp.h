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

	HINSTANCE AppInst() const;//����Ӧ��ʵ������Ŀ���
	HWND MainWnd() const;//���������ھ���Ŀ���
	float AspectRatio() const;//���غ��򻺳�Ŀ�߱�

	int Run();//ִ��Ӧ����Ϣѭ��

	virtual bool Init();//��ʼ����������Դ����ʼ����������ù�Դ��������InitMainWindow ��InitDirect3D
	virtual void OnResize();//��MsgProc�յ�WM_SIZE��Ϣʱ������������������ڴ�С�ı�ʱ��buffer�Ĵ�С����ͼ�Ĵ�С��ͶӰ����ȶ���Ҫ�������������������Ҫ�û��Լ�����
	virtual void UpdateScene(float dt) = 0;//ÿ֡������ã�����ÿ��dtʱ�����3DӦ��
	virtual void DrawScene();//ÿ֡���ã�������Ⱦ���ʵ�ʻ��Ƶ�ǰ֡�����򻺳壬������ɺ�����ĸ�present��ʾ����Ļ��
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);//Ϊ��Ӧ�ô���ִ�д��ڴ���������Ҫ�����Ϣʱ���������������

	//����¼�
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

protected:
	bool InitMainWindow();//��ʼ������
	bool InitDirect3D();//��ʼ��D3D
	//����FPS
	void CalculateFrameStats();

protected:
	HINSTANCE m_hAppInst;                //ʵ�����
	HWND m_hMainWnd;                     //�����ھ��
	bool m_isAppPaused;
	bool m_isMinimized;
	bool m_isMaximized;
	bool m_isResizing;
	UINT m_4xMsaaQuality;                //4�ز�������

	GameTimer m_timer;                   //��ʱ��
	D3D_DRIVER_TYPE m_d3dDriverType;     //��������
	ID3D11Device *m_pD3dDevice;          //�豸
	ID3D11DeviceContext *m_pImmediateContext;  //�豸������
	IDXGISwapChain  *m_pSwapChain;       //������
	ID3D11Texture2D *m_pDepthStencilBuffer;   //���ģ�建��
	ID3D11RenderTargetView *m_pRenderTargetView;  //��ȾĿ����ͼ
	ID3D11DepthStencilView *m_pDepthStencilView;  //���ģ�建����ͼ
	D3D11_VIEWPORT m_screenViewPort;      //�ӿ�

	std::wstring m_mainWndCaption;      //���ڱ���
	int m_clientWidth;
	int m_clientHeight;
	bool m_isEnable4xMsaa;              //�Ƿ�֧��4�ز���
};
#endif
