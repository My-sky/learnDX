#include "BlendingDemo.h"
#include "Effects.h"
#include "Vertex.h"
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Blending theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Blending::Blending(HINSTANCE hInstance)
	:D3DApp(hInstance), pHillVB(0), pHillIB(0),pWavesVB(0),pWavesIB(0),pBoxVB(0),pBoxIB(0),
	mGridIndexCount(0),pGrassMapSRV(0),pWaveMapSRV(0),pBoxMapSRV(0),mWaveTexOffset(0.0f,0.0f),
	mEyePosW(0.0f,0.0f,0.0f),mThea(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
mMainWndCaption = L"Blending Demo";
bIsEnable4xMsaa = false;

XMMATRIX I = XMMatrixIdentity();
XMStoreFloat4x4(&mHillWorld, I);
XMStoreFloat4x4(&mWaveWorld, I);
XMStoreFloat4x4(&mView, I);
XMStoreFloat4x4(&mProject, I);

XMMATRIX boxScale  = XMMatrixScaling(15.0f, 15.0f, 15.0f);
XMMATRIX boxOffset = XMMatrixTranslation(8.0f, 5.0f, -15.0f);
XMStoreFloat4x4(&mBoxWorld, boxScale*boxOffset);

XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

mLastMousePos.x = 0.0;
mLastMousePos.y = 0.0;

//Directional Light
mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

mHillMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mHillMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mHillMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

mWavesMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mWavesMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mBoxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mBoxMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}

Blending::~Blending()
{
	pImmediateContext->ClearState();
	ReleaseCOM(pHillVB);
	ReleaseCOM(pHillIB);
	ReleaseCOM(pWavesVB);
	ReleaseCOM(pWavesIB);
	ReleaseCOM(pBoxVB);
	ReleaseCOM(pBoxIB);

	ReleaseCOM(pGrassMapSRV);
	ReleaseCOM(pWaveMapSRV);
	ReleaseCOM(pBoxMapSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool Blending::Init()
{
	if (!D3DApp::Init())
		return false;

	mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

	Effects::InitAll(pd3dDevice);
	InputLayouts::InitAll(pd3dDevice);

	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/grass.dds", 0, 0, &pGrassMapSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/water2.dds", 0, 0, &pWaveMapSRV, 0));

	CreateHillGeometryBuffers();
	CreateWaveGeometryBuffers();


	return true;
}

void Blending::OnResize()
{
	D3DApp::OnResize();

	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Blending::UpdateScene(float dt)//update the view matrix ; the camera position
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

	Vertex::Basic32* v = reinterpret_cast<Vertex::Basic32*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);

		//derive tex-coords in [0,1] from position
		v[i].Tex.x = 0.5f + mWaves[i].x / mWaves.Width();
		v[i].Tex.y = 0.5f - mWaves[i].z / mWaves.Depth();//z的方向向里为正
	}

	pImmediateContext->Unmap(pWavesVB, 0);

	//animate the wave texture coordinates

	XMMATRIX mWaveScale = XMMatrixScaling(5.f, 5.f, 0.f);

	//Translate texture over time
	mWaveTexOffset.x += 0.05f*dt;
	mWaveTexOffset.y += 0.1f*dt;
	XMMATRIX mWaveTrans = XMMatrixTranslation(mWaveTexOffset.x, mWaveTexOffset.y, 0.f);


	//the spotlight takes on the camera position and is aimed in the 
	//same direction the camera is looking.In this way,it looks
	//like we are holding a flashlight
	XMStoreFloat4x4(&mWaveTexTransform, mWaveScale*mWaveTrans);
}

void Blending::DrawScene()
{
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::Blue));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	

	//constants
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	//XMMATRIX viewProj =  view *project;

	//set per frame constants
	Effects::pBasicFX->SetDirLights(mDirLights);
	Effects::pBasicFX->SetEyePosW(mEyePosW);

	
	//set state ,display the wireframe
	/*ID3D11RasterizerState* mWireframeRS;
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;

	HR(pd3dDevice->CreateRasterizerState(&rsDesc, &mWireframeRS));*/
	ID3DX11EffectTechnique* activeTech = Effects::pBasicFX->pLight3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the hills
		pImmediateContext->IASetVertexBuffers(0, 1, &pHillVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pHillIB, DXGI_FORMAT_R32_UINT, 0);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mHillWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMLoadFloat4x4(&mGrassTexTransform));
		Effects::pBasicFX->SetMaterial(mHillMat);
		Effects::pBasicFX->SetDiffuseMap(pGrassMapSRV);

		//display the wireframe
		//pImmediateContext->RSSetState(mWireframeRS);

		activeTech->GetPassByIndex(p)->Apply(0, pImmediateContext);

		//36 indices for the Wave
		pImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);

		//draw the waves
		pImmediateContext->IASetVertexBuffers(0, 1, &pWavesVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pWavesIB, DXGI_FORMAT_R32_UINT, 0);

		//set per object constants
		world = XMLoadFloat4x4(&mWaveWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMLoadFloat4x4(&mWaveTexTransform));
		Effects::pBasicFX->SetMaterial(mWavesMat);
		Effects::pBasicFX->SetDiffuseMap(pWaveMapSRV);
		

		activeTech->GetPassByIndex(p)->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);
	}

	HR(pSwapChain->Present(0, 0));

}

void Blending::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
}

void Blending::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Blending::OnMouseMove(WPARAM btnState, int x, int y)
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

float Blending::GetHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

XMFLOAT3 Blending::GetNormal(float x, float z) const
{
	//n=(-df/dx,1,-df/dz)
	XMFLOAT3 n(-0.03f*z*cosf(0.1f*x)-0.3f*cosf(0.1f*z),
				1.0f,
				-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));
	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);
	return n;
}

void Blending::CreateHillGeometryBuffers()
{
	//create vertex buffer
	MeshGenerator::MeshData grid;
	MeshGenerator meshGen;

	meshGen.CreateGrid(160.f, 160.f, 50, 50, grid);

	//the count of index
	mGridIndexCount = grid.Indices.size();

	//apply the height function to each vertex
	//allocate  color for each vertex
	std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
	for (int i = 0; i < grid.Vertices.size(); i++)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetNormal(p.x, p.z);
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
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
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pHillIB));
}

void Blending::CreateWaveGeometryBuffers()
{
	//create the vertex buffer.note that we allocate space only,as 
	//we will be updating the data every time step of the simulation

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * mWaves.VertexCount();
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
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k]     = i*n + j;
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
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pWavesIB));
}