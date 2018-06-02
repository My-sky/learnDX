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
	HINSTANCE hAppInst;                //ʵ�����
	HWND hMainWnd;                     //�����ھ��
	bool bIsAppPaused;
	bool bIsMinimized;
	bool bIsMaximized;
	bool bIsResizing;
	UINT m4xMsaaQuality;                //4�ز�������

	GameTimer mTimer;                   //��ʱ��
	D3D_DRIVER_TYPE md3dDriverType;     //��������
	ID3D11Device *pd3dDevice;          //�豸
	ID3D11DeviceContext *pImmediateContext;  //�豸������
	IDXGISwapChain  *pSwapChain;       //������
	ID3D11Texture2D *pDepthStencilBuffer;   //���ģ�建��
	ID3D11RenderTargetView *pRenderTargetView;  //��ȾĿ����ͼ
	ID3D11DepthStencilView *pDepthStencilView;  //���ģ�建����ͼ
	D3D11_VIEWPORT mScreenViewPort;      //�ӿ�

	std::wstring mMainWndCaption;      //���ڱ���
	int mClientWidth;
	int mClientHeight;
	bool bIsEnable4xMsaa;              //�Ƿ�֧��4�ز���
};
#endif
