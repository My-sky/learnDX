#include <windowsx.h>
#include <sstream>
#include"D3DApp.h"

namespace
{
	D3DApp *g_d3dApp = NULL;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_d3dApp->MsgProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance) :
	hAppInst(hInstance),
	mMainWndCaption(L"D3D11 Application"),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientHeight(600),
	mClientWidth(800),
	mEnable4xMsaa(false),
	hMainWnd(NULL),
	bIsAppPaused(false),
	bIsMaximized(false),
	bIsMinimized(false),
	bIsResizing(false),
	m4xMsaaQuality(0),

	pd3dDevice(NULL),
	pImmediateContext(NULL),
	pSwapChain(NULL),
	pRenderTargetView(NULL),
	pDepthStencilBuffer(NULL)
{
	ZeroMemory(&mScreenViewPort, sizeof(D3D11_VIEWPORT));
	g_d3dApp = this;
}

D3DApp::~D3DApp()
{
	ReleaseCOM(pRenderTargetView);
	ReleaseCOM(pDepthStencilBuffer);
	ReleaseCOM(pSwapChain);
	ReleaseCOM(pDepthStencilView);

	if (pImmediateContext)
		pImmediateContext->ClearState();

	ReleaseCOM(pImmediateContext);
	ReleaseCOM(pd3dDevice);
}

HINSTANCE D3DApp::AppInst() const
{
	return hAppInst;
}

HWND D3DApp::MainWnd() const
{
	return hMainWnd;
}

float D3DApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth / mClientHeight);
}

int D3DApp::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (!bIsAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return static_cast<int>(msg.wParam);
}

bool D3DApp::Init()
{
	if (!InitMainWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

void D3DApp::OnResize()
{
	assert(pImmediateContext);
	assert(pd3dDevice);
	assert(pSwapChain);

	//release old views
	ReleaseCOM(pRenderTargetView);
	ReleaseCOM(pDepthStencilView);
	ReleaseCOM(pDepthStencilBuffer);

	HR(pSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D *backBuffer;
	HR(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(pd3dDevice->CreateRenderTargetView(backBuffer, 0, &pRenderTargetView));
	ReleaseCOM(backBuffer);

	//create depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDecs;
	depthStencilDecs.Width     = mClientWidth;
	depthStencilDecs.Height    = mClientHeight;
	depthStencilDecs.MipLevels = 1;
	depthStencilDecs.ArraySize = 1;
	depthStencilDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//whether use 4x multisample
	if (mEnable4xMsaa)
	{
		depthStencilDecs.SampleDesc.Count = 4;
		depthStencilDecs.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDecs.SampleDesc.Count = 1;
		depthStencilDecs.SampleDesc.Quality = 0;
	}

	depthStencilDecs.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDecs.CPUAccessFlags = 0;
	depthStencilDecs.MiscFlags = 0;

	HR(pd3dDevice->CreateTexture2D(&depthStencilDecs, 0, &pDepthStencilBuffer));
	HR(pd3dDevice->CreateDepthStencilView(pDepthStencilBuffer,0, &pDepthStencilView));

	//bind the new render target view and depth/stencil view to the pipeline
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	//set viewport
	mScreenViewPort.TopLeftX = 0;
	mScreenViewPort.TopLeftY = 0;
	mScreenViewPort.Width = mClientWidth;
	mScreenViewPort.Height = mClientHeight;
	mScreenViewPort.MaxDepth = 1.0f;
	mScreenViewPort.MinDepth = 0.0f;

	pImmediateContext->RSSetViewports(1, &mScreenViewPort);
}

LRESULT D3DApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)//wParam一般存放与消息有关的常量值，窗口或控件的句柄；lParam存放指针的地址
{
	switch (msg)
	{
		//when a window is activity or inactivity
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			bIsAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			bIsAppPaused = false;
			mTimer.Start();
		}
		return 0;

		//resize the window
	case WM_SIZE:
		mClientWidth  = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (pd3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)//minimize the window
			{
				bIsAppPaused = true;
				bIsMinimized = true;
				bIsMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)//maximize the window
			{
				bIsAppPaused = false;
				bIsMinimized = false;
				bIsMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)//resize the window
			{
				if (bIsMinimized)
				{
					bIsAppPaused = false;
					bIsMinimized = false;
					OnResize();
				}
				else if (bIsMaximized)
				{
					bIsAppPaused = false;
					bIsMaximized = false;
					OnResize();
				}

				//call the OnResize() when user resized the window
				else if (bIsResizing)
				{

				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;
		//resize the window with dragging the window
	case WM_ENTERSIZEMOVE:
		bIsAppPaused = true;
		bIsResizing = true;
		mTimer.Stop();
		return 0;

		//user resized the window
	case WM_EXITSIZEMOVE:
		bIsAppPaused = false;
		bIsResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		//destroy the window
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		//the WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnenonic or
		//accelerator key.
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

		//avoid the window become too small
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;   
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hInstance = hAppInst;
	wc.lpfnWndProc = MainWndProc;
	wc.lpszClassName = L"D3DWndClassName";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClass(&wc))
   {
       MessageBox(0, L"RegisterClass Failed", 0, 0);
         return false;
   }

    RECT rect{ 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	hMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
         width, height, NULL, NULL, hAppInst, 0);
    if (!hMainWnd)
    {
        MessageBox(0, L"CreateWindow Failed", 0, 0);
         return 0;
     }

    ShowWindow(hMainWnd, SW_SHOW);
    UpdateWindow(hMainWnd);

    return true;

}

bool D3DApp::InitDirect3D()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,//default adapter
		md3dDriverType,
		0,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&pd3dDevice,
		&featureLevel,
		&pImmediateContext
	);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported!", 0, 0);
		return false;
	}

	//check 4x msaa quality support
	HR(pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	//
	//fill out the swapchain description
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = mClientWidth;
    sd.BufferDesc.Height = mClientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
     
	//check the 4x multisample
    if (mEnable4xMsaa)
     {
         sd.SampleDesc.Count = 4;
         sd.SampleDesc.Quality = m4xMsaaQuality - 1;
     }
    else
     {
         sd.SampleDesc.Count = 1;
         sd.SampleDesc.Quality = 0;
     }

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hMainWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//create the swapchain ,we need IDXGIFactory
	IDXGIDevice *pDxgiDevice = 0;
	HR(pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice)));

	IDXGIAdapter *pDxgiAdapter = 0;
	HR(pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter)));

	IDXGIFactory *pDxgiFactory = 0;
	HR(pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDxgiFactory)));

	HR(pDxgiFactory->CreateSwapChain(pd3dDevice, &sd, &pSwapChain));

	ReleaseCOM(pDxgiDevice);
	ReleaseCOM(pDxgiAdapter);
	ReleaseCOM(pDxgiFactory);

	OnResize();

	return true;
}

void D3DApp::CalculateFrameStats()
{
	//compute fps, called every frame
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frameCnt);
		float mspf = 1000.f / fps;

		std::wostringstream outs;
		outs.precision(6);//display 6 bit of float 
		outs << mMainWndCaption << L"   " << L"FPS:" << fps << L"   "
			<< L"Frame Time: " << mspf << L"  (ms)";
		SetWindowText(hMainWnd, outs.str().c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}

}

void D3DApp::DrawScene()
{

}
