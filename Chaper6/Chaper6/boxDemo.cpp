#include "boxDemo.h"

Box::Box(HINSTANCE hInstance)
	:D3DApp(hInstance),mBoxVB(0),mBoxIB(0),mFX(0),mTech(0),mfxWorldViewProject(0),mInputLayout(0),
	mThea(1.5f*MathHelper::Pi),mPhi(0.25f*MathHelper::Pi),mRadius(5.0f)
{
	m_mainWndCaption = L"Box Demo";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProject, I);

	mLastMousePos.x = 0.0;
	mLastMousePos.y = 0.0;

}

Box::~Box()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

bool Box::Init()
{
	if (!D3DApp::Init())
		return false;
	CreateGeometryBuffers();
	CreateFX();
	CreateLayout();

	return true;
}

void Box::OnResize()
{
	D3DApp::OnResize();
	
	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Box::UpdateScene(float dt)//update the view matrix ; the camera position
{
	//convert Spherical to Cartesian coordinates
	float x = mRadius * sinf(mPhi) * cosf(mThea);
	float z = mRadius * sinf(mPhi) * sinf(mThea);
	float y = mRadius * cosf(mPhi);

	//build view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Box::DrawScene()
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::Blue));
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pImmediateContext->IASetInputLayout(mInputLayout); 
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	//constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	XMMATRIX worldViewProj = world * view *project;

	mfxWorldViewProject->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, m_pImmediateContext);

		//36 indices for the box
		m_pImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(m_pSwapChain->Present(0, 0));

}

void Box::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void Box::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Box::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(
			0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(
			0.25f*static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around box.
		mThea += dx;
		mPhi  += dy;
		//Restrict the angle mPhi
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if((btnState & MK_RBUTTON)!=0)
	{
		//Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		//Update the camera radius based on input.
		mRadius += dx - dy;

		//Restric the radius
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Box::CreateGeometryBuffers()
{
	//create vertex buffer
	Vertex vertex[] =
	{
		{XMFLOAT3(-1.0f,-1.0f,-1.0f),(const float*)&Colors::White},
		{XMFLOAT3(-1.0f,+1.0f,-1.0f),(const float*)&Colors::Black},
		{XMFLOAT3(+1.0f,+1.0f,-1.0f),(const float*)&Colors::Red },
		{XMFLOAT3(+1.0f,-1.0f,-1.0f),(const float*)&Colors::Green },
		{XMFLOAT3(-1.0f,-1.0f,+1.0f),(const float*)&Colors::Blue },
		{XMFLOAT3(-1.0f,+1.0f,+1.0f),(const float*)&Colors::Yellow },
		{XMFLOAT3(+1.0f,+1.0f,+1.0f),(const float*)&Colors::Cyan },
		{XMFLOAT3(+1.0f,-1.0f,+1.0f),(const float*)&Colors::Magenta},
	};

	D3D11_BUFFER_DESC vbd;

}