#include "StencilingDemo.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Stenciling theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Stenciling::Stenciling(HINSTANCE hInstance)
	:D3DApp(hInstance), pRoomVB(0),pSkullVB(0),pSkullIB(0),
	mSkullIndexCount(0),pWallDiffuseMapSRV(0),pFloorDiffuseMapSRV(0),pMirrorDiffuseMapSRV(0),mRenderOptions(RenderOptions::TextureAndFog),
	mSkullTranslation(0.0f,0.0f,-3.0f),mEyePosW(0.0f,0.0f,0.0f),mThea(1.24f*MathHelper::Pi), mPhi(0.42f*MathHelper::Pi), mRadius(12.0f)
{
mMainWndCaption = L"Stenciling Demo";
bIsEnable4xMsaa = false;

XMMATRIX I = XMMatrixIdentity();
XMStoreFloat4x4(&mRoomWorld, I);
XMStoreFloat4x4(&mView, I);
XMStoreFloat4x4(&mProject, I);

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

mRoomMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mRoomMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mRoomMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

mSkullMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mSkullMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mSkullMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

mMirrorMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mMirrorMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
mMirrorMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}

Stenciling::~Stenciling()
{
	pImmediateContext->ClearState();
	ReleaseCOM(pRoomVB);
	ReleaseCOM(pSkullVB);
	ReleaseCOM(pSkullIB);

	ReleaseCOM(pWallDiffuseMapSRV);
	ReleaseCOM(pFloorDiffuseMapSRV);
	ReleaseCOM(pMirrorDiffuseMapSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool Stenciling::Init()
{
	if (!D3DApp::Init())
		return false;

	Effects::InitAll(pd3dDevice);
	InputLayouts::InitAll(pd3dDevice);
	RenderStates::InitAll(pd3dDevice);

	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/checkboard.dds", 0, 0, &pFloorDiffuseMapSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/brick01.dds", 0, 0, &pWallDiffuseMapSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/ice.dds", 0, 0, &pMirrorDiffuseMapSRV, 0));

	CreateRoomGeometryBuffers();
	CreateSkullGeometryBuffers();

	return true;
}

void Stenciling::OnResize()
{
	D3DApp::OnResize();

	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Stenciling::UpdateScene(float dt)//update the view matrix ; the camera position
{
	//convert Spherical to Cartesian coordinates
	float x = mRadius * sinf(mPhi) * cosf(mThea);
	float z = mRadius * sinf(mPhi) * sinf(mThea);
	float y = mRadius * cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	//build view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);


	//Switch the render mode based in key input
	if (GetAsyncKeyState('1') & 0x8000)
		mRenderOptions = RenderOptions::Lighting;
	if (GetAsyncKeyState('2') & 0x8000)
		mRenderOptions = RenderOptions::Textures;
	if (GetAsyncKeyState('3') & 0x8000)
		mRenderOptions = RenderOptions::TextureAndFog;

	//allow user to move skull
	if (GetAsyncKeyState('A') & 0x8000)
		mSkullTranslation.x -= 1.0f*dt;
	if (GetAsyncKeyState('D') & 0x8000)
		mSkullTranslation.x += 1.0f*dt;
	if (GetAsyncKeyState('W') & 0x8000)
		mSkullTranslation.z += 1.0f*dt;
	if (GetAsyncKeyState('S') & 0x8000)
		mSkullTranslation.z -= 1.0f*dt;

	//set the downline of the move
	mSkullTranslation.y = MathHelper::Max(mSkullTranslation.y, 0.0f);

	//update the translation matrix
	XMMATRIX skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
	XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
	XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
	XMStoreFloat4x4(&mSkullWorld, skullRotate*skullScale*skullOffset);
}

void Stenciling::DrawScene()
{
	pImmediateContext->ClearRenderTargetView(pRenderTargetView,
		reinterpret_cast<const float*>(&Colors::Silver));
	pImmediateContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	
	float blendFactor[] = { 0.0f,0.0f,0.0f,0.0f };

	//constants
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX project = XMLoadFloat4x4(&mProject);
	//XMMATRIX viewProj =  view *project;

	//set per frame constants
	Effects::pBasicFX->SetDirLights(mDirLights);
	Effects::pBasicFX->SetEyePosW(mEyePosW);
	Effects::pBasicFX->SetFogColor(Colors::Silver);
	Effects::pBasicFX->SetFogStart(2.0f);
	Effects::pBasicFX->SetFogRange(40.0f);

	
	//set state ,display the wireframe
	/*ID3D11RasterizerState* mWireframeRS;
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;

	HR(pd3dDevice->CreateRasterizerState(&rsDesc, &mWireframeRS));*/
	ID3DX11EffectTechnique* activeTech=NULL;
	ID3DX11EffectTechnique* activeSkullTech=NULL;

	switch (mRenderOptions)
	{
	case RenderOptions::Lighting:
		activeTech = Effects::pBasicFX->pLight3Tech;
		activeSkullTech = Effects::pBasicFX->pLight3Tech;
		break;
	case RenderOptions::Textures:
		activeTech = Effects::pBasicFX->pLight3TexTech;
		activeSkullTech = Effects::pBasicFX->pLight3Tech;
		break;
	case RenderOptions::TextureAndFog:
		activeTech = Effects::pBasicFX->pLight3TexFogTech;
		activeSkullTech = Effects::pBasicFX->pLight3FogTech;
		break;
	}

	D3DX11_TECHNIQUE_DESC techDesc;

	////draw the floor and wall to the back buffer

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the hills
		pImmediateContext->IASetVertexBuffers(0, 1, &pRoomVB, &stride, &offset);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mRoomWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::pBasicFX->SetMaterial(mRoomMat);

		//floor
		Effects::pBasicFX->SetDiffuseMap(pFloorDiffuseMapSRV);
		activeTech->GetPassByIndex(p)->Apply(0, pImmediateContext);
		pImmediateContext->Draw(6, 0);

		//wall
		Effects::pBasicFX->SetDiffuseMap(pWallDiffuseMapSRV);
		activeTech->GetPassByIndex(p)->Apply(0, pImmediateContext);
		pImmediateContext->Draw(18, 6);

	}

	//draw skull
	activeSkullTech->GetDesc(&techDesc);
	for(UINT i= 0;i<techDesc.Passes;i++)
	{
		//draw hill
		pImmediateContext->IASetVertexBuffers(0, 1, &pSkullVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pSkullIB, DXGI_FORMAT_R32_UINT, 0);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mSkullWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetMaterial(mSkullMat);

		//display the wireframe
		activeSkullTech->GetPassByIndex(i)->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	}

	//draw the mirror to the stencil buffer
	activeTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		pImmediateContext->IASetVertexBuffers(0, 1, &pRoomVB, &stride, &offset);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mRoomWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMMatrixIdentity());

		//do not write to the render target
		pImmediateContext->OMSetBlendState(RenderStates::pNoRenderTargetWritesBS, blendFactor, 0xffffffff);
		//write the stencil buffer
		pImmediateContext->OMSetDepthStencilState(RenderStates::pMarkMirrorDSS, 1);
		activeTech->GetPassByIndex(i)->Apply(0,pImmediateContext);
		pImmediateContext->Draw(6, 24);

		pImmediateContext->OMSetDepthStencilState(0, 0);
		pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	//draw the skull reflection.
	activeSkullTech->GetDesc(&techDesc);
	for (UINT i = 0; i<techDesc.Passes; i++)
	{
		//draw hill
		pImmediateContext->IASetVertexBuffers(0, 1, &pSkullVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pSkullIB, DXGI_FORMAT_R32_UINT, 0);

		//Set per object constants
		XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMMATRIX R = XMMatrixReflect(mirrorPlane);
		XMMATRIX world = XMLoadFloat4x4(&mSkullWorld)*R;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetMaterial(mSkullMat);

		XMFLOAT3 oldLightDirections[3];
		for (UINT i = 0; i < 3; i++)
		{
			oldLightDirections[i] = mDirLights[i].Direction;

			XMVECTOR lightDir = XMLoadFloat3(&mDirLights[i].Direction);
			XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
			XMStoreFloat3(&mDirLights[i].Direction, reflectedLightDir);
		}

		Effects::pBasicFX->SetDirLights(mDirLights);

		//cull clockwise triangle for reflection
		pImmediateContext->RSSetState(RenderStates::pCullClockwiseRS);

		pImmediateContext->OMSetDepthStencilState(RenderStates::pDrawReflectionDSS, 1);
		activeSkullTech->GetPassByIndex(i)->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);

		//reset states
		pImmediateContext->RSSetState(0);
		pImmediateContext->OMSetDepthStencilState(0, 0);

		//restore light directions
		for (UINT i = 0; i < 3; i++)
		{
			mDirLights[i].Direction = oldLightDirections[i];
		}
		Effects::pBasicFX->SetDirLights(mDirLights);
	}

	////draw the mirror to the back buffer as usual 
	activeTech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		pImmediateContext->IASetVertexBuffers(0, 1, &pRoomVB, &stride, &offset);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mRoomWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::pBasicFX->SetMaterial(mMirrorMat);
		Effects::pBasicFX->SetDiffuseMap(pMirrorDiffuseMapSRV);

		//do not write to the render target
		pImmediateContext->OMSetBlendState(RenderStates::pTransparentBS, blendFactor, 0xffffffff);
		activeTech->GetPassByIndex(i)->Apply(0, pImmediateContext);
		pImmediateContext->Draw(6, 24);

		pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	HR(pSwapChain->Present(0, 0));

}

void Stenciling::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
}

void Stenciling::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Stenciling::OnMouseMove(WPARAM btnState, int x, int y)
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
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 50.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Stenciling::CreateRoomGeometryBuffers()
{
	//create vertex buffer
	Vertex::Basic32 v[30];

	//floor:observe we tile texture coordinates
	v[0] = Vertex::Basic32(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	v[1] = Vertex::Basic32(-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex::Basic32(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);

	v[3] = Vertex::Basic32(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	v[4] = Vertex::Basic32(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
	v[5] = Vertex::Basic32(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f);

	//wall
	v[6] = Vertex::Basic32(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[7] = Vertex::Basic32(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex::Basic32(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);

	v[9] = Vertex::Basic32(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[10] = Vertex::Basic32(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
	v[11] = Vertex::Basic32(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f);

	v[12] = Vertex::Basic32(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[13] = Vertex::Basic32(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex::Basic32(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);

	v[15] = Vertex::Basic32(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[16] = Vertex::Basic32(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	v[17] = Vertex::Basic32(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	v[18] = Vertex::Basic32(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex::Basic32(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex::Basic32(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);

	v[21] = Vertex::Basic32(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex::Basic32(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
	v[23] = Vertex::Basic32(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f);

	//mirror
	v[24] = Vertex::Basic32(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[25] = Vertex::Basic32(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[26] = Vertex::Basic32(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[27] = Vertex::Basic32(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[28] = Vertex::Basic32(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[29] = Vertex::Basic32(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * 30;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pRoomVB));

}

void Stenciling::CreateSkullGeometryBuffers()
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT	vcount = 0;
	UINT	tcount = 0;

	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex::Basic32> vertices(vcount);
	for (UINT i = 0; i < vcount; i++)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3 * tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for (UINT i = 0; i < tcount; i++)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();


	//create the vertex buffer.note that we allocate space only,as 
	//we will be updating the data every time step of the simulation

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) *vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA viniData;
	viniData.pSysMem = &vertices[0];
	HR(pd3dDevice->CreateBuffer(&vbd, &viniData, &pSkullVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pSkullIB));
}
