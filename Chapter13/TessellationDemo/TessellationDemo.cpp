#include "TessellationDemo.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Tessellation theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Tessellation::Tessellation(HINSTANCE hInstance)
	:D3DApp(hInstance),pTessVB(0),pTessLayout(0),mEyePosW(0.0f,0.0f,0.0f),mThea(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
mMainWndCaption = L"Tessellation Demo";
mEnable4xMsaa = false;

XMMATRIX I = XMMatrixIdentity();
XMStoreFloat4x4(&mView, I);
XMStoreFloat4x4(&mProject, I);

mLastMousePos.x = 0.0;
mLastMousePos.y = 0.0;

}

Tessellation::~Tessellation()
{
	pImmediateContext->ClearState();

	ReleaseCOM(pTessVB);
	ReleaseCOM(pTessLayout);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool Tessellation::Init()
{
	if (!D3DApp::Init())
		return false;


	Effects::InitAll(pd3dDevice);
	InputLayouts::InitAll(pd3dDevice);
	RenderStates::InitAll(pd3dDevice);
	CreateTessVB();
	CreateTessLayout();

	return true;
}

void Tessellation::OnResize()
{
	D3DApp::OnResize();

	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Tessellation::UpdateScene(float dt)//update the view matrix ; the camera position
{
	//convert Spherical to Cartesian coordinates
	float x = mRadius * sinf(mPhi) * cosf(mThea);
	float z = mRadius * sinf(mPhi) * sinf(mThea);
	float y = mRadius * cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	//build view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	
}

void Tessellation::DrawScene()
{
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::Silver));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	XMMATRIX viewProj = view *project;


	pImmediateContext->IASetInputLayout(pTessLayout);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	
	float blendFactor[] = { 0.0f,0.0f,0.0f,0.0f };

	//set per frame constants
	Effects::pTessellationFX->SetEyePosW(mEyePosW);
	Effects::pTessellationFX->SetFogColor(Colors::Silver);
	Effects::pTessellationFX->SetFogStart(15.0f);
	Effects::pTessellationFX->SetFogRange(175.0f);

	ID3DX11EffectTechnique* tessTech=NULL;

	D3DX11_TECHNIQUE_DESC techDesc;

	////draw box with clipping
	tessTech = Effects::pTessellationFX->pTessTech;

	tessTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the hills
		pImmediateContext->IASetVertexBuffers(0, 1, &pTessVB, &stride, &offset);

		//Set per object constants
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pTessellationFX->SetWorld(world);
		Effects::pTessellationFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pTessellationFX->SetWorldViewProject(worldViewProj);

		//display the wireframe
		
		tessTech->GetPassByIndex(p)->Apply(0, pImmediateContext);

		pImmediateContext->RSSetState(RenderStates::pNoCullRS);
		pImmediateContext->RSSetState(RenderStates::pWireframeRS);
		pImmediateContext->Draw(16, 0);

		pImmediateContext->RSSetState(0);
	}

	HR(pSwapChain->Present(0, 0));

}

void Tessellation::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
}

void Tessellation::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Tessellation::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around Wave.
		mThea += dx;
		mPhi += dy;
		//Restrict the angle mPhi
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.1f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.1f*static_cast<float>(y - mLastMousePos.y);

		//Update the camera radius based on input.
		mRadius += dx - dy;

		//Restric the radius
		mRadius = MathHelper::Clamp(mRadius, 10.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Tessellation::CreateTessLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
			D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	D3DX11_PASS_DESC passDesc;
	Effects::pTessellationFX->pTessTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pd3dDevice->CreateInputLayout(desc, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pTessLayout));
}

void Tessellation::CreateTessVB()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * 16;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	XMFLOAT3 vertices[] =
	{
		/*XMFLOAT3(1.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,0.0f,1.0f),
		XMFLOAT3(0.0f,0.0f,1.0f)*/

		//Bezer
		// Row 0
		XMFLOAT3(-10.0f, -10.0f, +15.0f),
		XMFLOAT3(-5.0f,  0.0f, +15.0f),
		XMFLOAT3(+5.0f,  0.0f, +15.0f),
		XMFLOAT3(+10.0f, 0.0f, +15.0f),

		// Row 1
		XMFLOAT3(-15.0f, 0.0f, +5.0f),
		XMFLOAT3(-5.0f,  0.0f, +5.0f),
		XMFLOAT3(+5.0f,  20.0f, +5.0f),
		XMFLOAT3(+15.0f, 0.0f, +5.0f),

		// Row 2
		XMFLOAT3(-15.0f, 0.0f, -5.0f),
		XMFLOAT3(-5.0f,  0.0f, -5.0f),
		XMFLOAT3(+5.0f,  0.0f, -5.0f),
		XMFLOAT3(+15.0f, 0.0f, -5.0f),

		// Row 3
		XMFLOAT3(-10.0f, 10.0f, -15.0f),
		XMFLOAT3(-5.0f,  0.0f, -15.0f),
		XMFLOAT3(+5.0f,  0.0f, -15.0f),
		XMFLOAT3(+25.0f, 10.0f, -15.0f)
	};

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pTessVB));
}