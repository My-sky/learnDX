#include "LightingDemo.h"
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Lighting theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Lighting::Lighting(HINSTANCE hInstance)
	:D3DApp(hInstance), pHillVB(0), pHillIB(0),pWaveVB(0),pWaveIB(0),
	pFX(0), pTech(0), pInputLayout(0),mGridIndexCount(0),
	pfxWorld(0),pfxWorldInvTranspose(0),pfxEyePosW(0),pfxWorldViewProject(0),
	pfxDirLight(0),pfxPointLight(0),pfxSpotLight(0),pfxMaterial(0),
	mEyePosW(0.0f,0.0f,0.0f),mThea(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(80.0f)
{
	mMainWndCaption = L"Lighting Demo";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mHillWorld, I);
	XMStoreFloat4x4(&mWaveWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProject, I);

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	XMStoreFloat4x4(&mWaveWorld, wavesOffset);

	mLastMousePos.x = 0.0;
	mLastMousePos.y = 0.0;

	//Directional Light
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);


}

Lighting::~Lighting()
{
	ReleaseCOM(pLightingVB);
	ReleaseCOM(pLightingIB);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
}

bool Lighting::Init()
{
	if (!D3DApp::Init())
		return false;
	CreateGeometryBuffers();
	CreateFX();
	CreateLayout();

	return true;
}

void Lighting::OnResize()
{
	D3DApp::OnResize();

	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Lighting::UpdateScene(float dt)//update the view matrix ; the camera position
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

void Lighting::DrawScene()
{
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::cBlue));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pImmediateContext->IASetInputLayout(pInputLayout);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(CommonVertex);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pLightingVB, &stride, &offset);
	pImmediateContext->IASetIndexBuffer(pLightingIB, DXGI_FORMAT_R32_UINT, 0);

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

		//36 indices for the Lighting
		pImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);
	}

	HR(pSwapChain->Present(0, 0));

}

void Lighting::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
}

void Lighting::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Lighting::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around Lighting.
		mThea += dx;
		mPhi += dy;
		//Restrict the angle mPhi
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

		//Update the camera radius based on input.
		mRadius += dx - dy;

		//Restric the radius
		mRadius = MathHelper::Clamp(mRadius, 50.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float Lighting::GetHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

void Lighting::CreateGeometryBuffers()
{
	//create vertex buffer
	MeshGenerator::MeshData grid;
	MeshGenerator meshGen;

	meshGen.CreateGrid(160.f, 160.f, 50, 50, grid);

	//the count of index
	mGridIndexCount = grid.Indices.size();

	//apply the height function to each vertex
	//allocate  color for each vertex
	std::vector<CommonVertex> vertices(grid.Vertices.size());
	for (int i = 0; i < grid.Vertices.size(); i++)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		p.y = GetHeight(p.x, p.z);

		vertices[i].pos = p;
		//allocate color base on the height
		if (p.y < -10.f)
		{
			//sandy beach color
			vertices[i].color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if(p.y < 5.f)
		{
			//light yellow-green
			vertices[i].color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (p.y < 12.f)
		{
			//dark yellow-green
			vertices[i].color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (p.y < 20.f)
		{
			//dark brown
			vertices[i].color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			//white
			vertices[i].color = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(CommonVertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pLightingVB));

	//create the index buffer

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pLightingIB));
}

void Lighting::CreateFX()
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
	std::ifstream fin("..\\shaders\\Lightingcolor.fxo", std::ios::binary);

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

void Lighting::CreateLayout()
{
	//Create the vertex input layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//Create input layout
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pd3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout));
}