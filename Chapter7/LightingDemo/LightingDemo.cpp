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
	:D3DApp(hInstance), pHillVB(0), pHillIB(0),pWavesVB(0),pWavesIB(0),
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
mDirLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

//Point light--position is changed every frame to animate in UpdateScene function
mPointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
mPointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
mPointLight.Range = 25.0f;

// Spot light--position and direction changed every frame to animate in UpdateScene function.
mSpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
mSpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mSpotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
mSpotLight.Spot = 96.0f;
mSpotLight.Range = 10000.0f;

mHillMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
mHillMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
mHillMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

mWavesMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
mWavesMat.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);


}

Lighting::~Lighting()
{
	ReleaseCOM(pHillVB);
	ReleaseCOM(pHillIB);
	ReleaseCOM(pWavesVB);
	ReleaseCOM(pWavesIB);

	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
}

bool Lighting::Init()
{
	if (!D3DApp::Init())
		return false;

	mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

	CreateHillGeometryBuffers();
	CreateWaveGeometryBuffers();
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

	mEyePosW = XMFLOAT3(x, y, z);

	//build view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	//every quarter second,generate a random wave
	static float t_base = 0.0f;
	if ((mTimer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;
		DWORD i = 5 + rand() % (mWaves.RowCount() - 10);
		DWORD j = 5 + rand() % (mWaves.ColumnCount() - 10);

		float r = MathHelper::RandF(1.0f, 2.0f);
		mWaves.Disturb(i, j, r);
	}
	mWaves.Update(dt);

	//Update the wave vertex buffer with the new solution
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(pImmediateContext->Map(pWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	CommonVertex* v = reinterpret_cast<CommonVertex*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].pos = mWaves[i];
		v[i].normal = mWaves.Normal(i);
	}
	pImmediateContext->Unmap(pWavesVB, 0);

	//animate the lights

	//circle light over the land surface
	mPointLight.Position.x = 70.0f*cosf(0.2f*mTimer.TotalTime());
	mPointLight.Position.z = 70.0f*cosf(0.2f*mTimer.TotalTime());
	mPointLight.Position.y = MathHelper::Max(GetHeight(mPointLight.Position.x, mPointLight.Position.z), -3.0f) + 10.0f;

	//the spotlight takes on the camera position and is aimed in the 
	//same direction the camera is looking.In this way,it looks
	//like we are holding a flashlight
	mSpotLight.Position = mEyePosW;
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));
}

void Lighting::DrawScene()
{
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::cLightSteelBlue));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pImmediateContext->IASetInputLayout(pInputLayout);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(CommonVertex);
	UINT offset = 0;
	

	//constants
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	XMMATRIX viewProj =  view *project;

	//set per frame constants
	pfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
	pfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));
	pfxSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));
	pfxEyePosW->SetRawValue(&mEyePosW, 0, sizeof(mEyePosW));

	
	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the hills
		pImmediateContext->IASetVertexBuffers(0, 1, &pHillVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pHillIB, DXGI_FORMAT_R32_UINT, 0);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mHillWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		pfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		pfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
		pfxWorldViewProject->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		pfxMaterial->SetRawValue(&mHillMat, 0, sizeof(mHillMat));


		pTech->GetPassByIndex(p)->Apply(0, pImmediateContext);

		//36 indices for the Lighting
		pImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);

		//draw the waves
		pImmediateContext->IASetVertexBuffers(0, 1, &pWavesVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pWavesIB, DXGI_FORMAT_R32_UINT, 0);

		//set per object constants
		world = XMLoadFloat4x4(&mWaveWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*project;

		pfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		pfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
		pfxWorldViewProject->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		pfxMaterial->SetRawValue(&mWavesMat, 0, sizeof(mWavesMat));

		pTech->GetPassByIndex(p)->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);
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

XMFLOAT3 Lighting::GetNormal(float x, float z) const
{
	//n=(-df/dx,1,-df/dz)
	XMFLOAT3 n(-0.03f*z*cosf(0.1f*x)-0.03f*cosf(0.1f*z),
				1.0f,
				-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));
	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);
	return n;
}

void Lighting::CreateHillGeometryBuffers()
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
		vertices[i].normal = GetNormal(p.x, p.y);
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
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pHillVB));

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
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pHillIB));
}

void Lighting::CreateWaveGeometryBuffers()
{
	//create the vertex buffer.note that we allocate space only,as 
	//we will be updating the data every time step of the simulation

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CommonVertex) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(pd3dDevice->CreateBuffer(&vbd, 0, &pWavesVB));

	//create the index buffer.the index buffer is fixed,so we
	//only need to create and set once

	std::vector<UINT> indices(3 * mWaves.TriangleCount());

	//Iterator over each quad
	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			indices[k]   = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i+1)*n + j;

			indices[k + 3] = (i+1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i+1)*n + j + 1;

			k += 6;
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pHillIB));
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
	std::ifstream fin("..\\shaders\\Lighting.fxo", std::ios::binary);

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

	pTech					= pFX->GetTechniqueByName("ColorTech");
	pfxWorldViewProject		= pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	pfxWorld				= pFX->GetVariableByName("gWorld")->AsMatrix();
	pfxWorldInvTranspose	= pFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	pfxEyePosW				= pFX->GetVariableByName("gEyePosW")->AsVector();
	pfxDirLight				= pFX->GetVariableByName("gDirLight");
	pfxPointLight			= pFX->GetVariableByName("gPointLight");
	pfxSpotLight			= pFX->GetVariableByName("gSpotLight");
	pfxMaterial				= pFX->GetVariableByName("gMaterial");
}

void Lighting::CreateLayout()
{
	//Create the vertex input layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//Create input layout
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pd3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout));
}