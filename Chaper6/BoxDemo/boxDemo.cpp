#include "boxDemo.h"
#include <fstream>
#include <vector>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Box theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Box::Box(HINSTANCE hInstance)
	:D3DApp(hInstance),pBoxVB(0),pBoxIB(0),pFX(0),pTech(0),pfxWorldViewProject(0),pInputLayout(0),
	mThea(1.5f*MathHelper::Pi),mPhi(0.25f*MathHelper::Pi),mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProject, I);

	mLastMousePos.x = 0.0;
	mLastMousePos.y = 0.0;

}

Box::~Box()
{
	ReleaseCOM(pBoxVB);
	ReleaseCOM(pBoxIB);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
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
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::cBlue));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pImmediateContext->IASetInputLayout(pInputLayout); 
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pBoxVB, &stride, &offset);
	pImmediateContext->IASetIndexBuffer(pBoxIB, DXGI_FORMAT_R32_UINT, 0);

	//constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	XMMATRIX worldViewProj = world * view *project;

	pfxWorldViewProject->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pImmediateContext);

		//36 indices for the box
		pImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(pSwapChain->Present(0, 0));

}

void Box::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
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
		{XMFLOAT3(-1.0f,-1.0f,-1.0f),(const float*)&Colors::cWhite},
		{XMFLOAT3(-1.0f,+1.0f,-1.0f),(const float*)&Colors::cBlack},
		{XMFLOAT3(+1.0f,+1.0f,-1.0f),(const float*)&Colors::cRed },
		{XMFLOAT3(+1.0f,-1.0f,-1.0f),(const float*)&Colors::cGreen },
		{XMFLOAT3(-1.0f,-1.0f,+1.0f),(const float*)&Colors::cBlue },
		{XMFLOAT3(-1.0f,+1.0f,+1.0f),(const float*)&Colors::cYellow },
		{XMFLOAT3(+1.0f,+1.0f,+1.0f),(const float*)&Colors::cCyan },
		{XMFLOAT3(+1.0f,-1.0f,+1.0f),(const float*)&Colors::cMagenta},
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertex;
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pBoxVB));

	//create the index buffer
	UINT indices[] =
	{
		//front face
		0,1,2,
		0,2,3,

		//back face
		4,6,5,
		4,7,6,

		//left face
		4,5,1,
		4,1,0,

		//right face
		3,2,6,
		3,6,7,

		//top face
		1,5,6,
		1,6,2,

		//bottom face
		4,0,3,
		4,3,7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pBoxIB));
}

void Box::CreateFX()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* pCompiledShader = 0;
	ID3D10Blob* pCompilationMsgs = 0;

	//Compile at runtime
	//HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &pCompiledShader, &pCompilationMsgs, 0);
	////compilationMsgs store errors or warnings
	//if (pCompilationMsgs != 0)
	//{
	//	MessageBoxA(0, (char*)pCompilationMsgs->GetBufferPointer(), 0, 0);
	//	ReleaseCOM(pCompilationMsgs);
	//}

	////check other errors
	//if (FAILED(hr))
	//{
	//	DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	//}

	//compile at build 
	std::ifstream fin("..\\shaders\\color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> vCompiledShader(size);

	fin.read(&vCompiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(
		&vCompiledShader[0],
		size,
		0, pd3dDevice, &pFX));

	//Release compiled shader
	ReleaseCOM(pCompiledShader);

	pTech = pFX->GetTechniqueByName("ColorTech");
	pfxWorldViewProject = pFX->GetVariableByName("gWorldViewProj")->AsMatrix();

}

void Box::CreateLayout()
{
	//Create the vertex input layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[]=
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	//Create input layout
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pd3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout));
}