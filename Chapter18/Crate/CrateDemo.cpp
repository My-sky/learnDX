#include "CrateDemo.h"
#include <fstream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Crate theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

Crate::Crate(HINSTANCE hInstance)
	:D3DApp(hInstance), pBoxVB(0),pBoxIB(0),pDiffuseMapSRV(0),pNormalMapSRV(0),
	mEyePosW(0.0f,0.0f,0.0f),mThea(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f)
{
mMainWndCaption = L"Crate Demo";

XMMATRIX I = XMMatrixIdentity();
XMStoreFloat4x4(&mBoxWorld, I);
XMStoreFloat4x4(&mTexTransform, I);
XMStoreFloat4x4(&mView, I);
XMStoreFloat4x4(&mProject, I);

mLastMousePos.x = 0.0;
mLastMousePos.y = 0.0;

//Directional Light
mDirLights.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
mDirLights.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
mDirLights.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
mDirLights.Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

//mDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
//mDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
//mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
//mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
mBoxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}

Crate::~Crate()
{
	ReleaseCOM(pBoxVB);
	ReleaseCOM(pBoxIB);
	ReleaseCOM(pDiffuseMapSRV);
	ReleaseCOM(pNormalMapSRV);
	ReleaseCOM(pHeightMapSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool Crate::Init()
{
	if (!D3DApp::Init())
		return false;

	Effects::InitAll(pd3dDevice);
	InputLayouts::InitAll(pd3dDevice);

	D3DX11_IMAGE_LOAD_INFO info;
	info.MipLevels = 1;
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"Texture/bricks.jpg", &info, 0, &pDiffuseMapSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"Texture/bricks_normal.jpg", 0, 0, &pNormalMapSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"Texture/bricks_disp.jpg", 0, 0, &pHeightMapSRV, 0));

	CreateGeometryBuffers();
	return true;
}

void Crate::OnResize()
{
	D3DApp::OnResize();

	//the window is changed,reset the projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProject, P);
}

void Crate::UpdateScene(float dt)//update the view matrix ; the camera position
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



	////rotate the texture
	//float tempTime = mTimer.TotalTime();
	//float thea = tempTime*MathHelper::Pi;// / 30;//×ª»»³É»¡¶È
	//XMFLOAT4X4 mTrans(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 1.0f);
	//XMFLOAT4X4 mRotate(cos(thea), sin(thea), 0.0f, 0.0f, -sin(thea), cos(thea), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f);
	//XMFLOAT4X4 mScale(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	//XMMATRIX trans = XMLoadFloat4x4(&mTrans);
	//XMMATRIX rotate = XMLoadFloat4x4(&mRotate);
	//XMMATRIX texTrans = trans * rotate;//XMMatrixTranspose
	////XMStoreFloat4x4(&mTexTransform, texTrans);

}

void Crate::DrawScene()
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
	XMMATRIX viewProj =  view *project;

	//set per frame constants
	Effects::pBasicFX->SetDirLights(mDirLights);
	Effects::pBasicFX->SetEyePosW(mEyePosW);
	Effects::pBasicFX->SetMinTessDistance(25.0f);
	Effects::pBasicFX->SetMaxTessDistance(1.0f);
	Effects::pBasicFX->SetMinTessFactor(1.0f);
	Effects::pBasicFX->SetMaxTessFactor(5.0f);
	Effects::pBasicFX->SetHeightScale(0.07f);

	//render mode 
	ID3D11RasterizerState* pWireframeRS;
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(pd3dDevice->CreateRasterizerState(&wireframeDesc, &pWireframeRS));


	ID3DX11EffectTechnique* pTech = Effects::pBasicFX->pLight2TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the hills
		pImmediateContext->IASetVertexBuffers(0, 1, &pBoxVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pBoxIB, DXGI_FORMAT_R32_UINT, 0);

		//Set per object constants
		XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*project;

		Effects::pBasicFX->SetWorld(world);
		Effects::pBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::pBasicFX->SetWorldViewProject(worldViewProj);
		Effects::pBasicFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
		Effects::pBasicFX->SetMaterial(mBoxMat);
		Effects::pBasicFX->SetDiffuseMap(pDiffuseMapSRV);
		Effects::pBasicFX->SetNormalMap(pNormalMapSRV);
		Effects::pBasicFX->SetHeightMap(pHeightMapSRV);

		pTech->GetPassByIndex(p)->Apply(0, pImmediateContext);
		
		//pImmediateContext->RSSetState(pWireframeRS);
		//36 indices for the Crate
		pImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
		//pImmediateContext->RSSetState(0);
		
	}

	HR(pSwapChain->Present(0, 0));
	ReleaseCOM(pWireframeRS);
}

void Crate::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(hMainWnd);
}

void Crate::OnMouseUp(WPARAM	btnState, int x, int y)
{
	ReleaseCapture();
}

void Crate::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around Crate.
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
		mRadius = MathHelper::Clamp(mRadius, 1.0f, 50.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Crate::CreateGeometryBuffers()
{
	//create vertex buffer
	MeshGenerator::MeshData box;
	MeshGenerator meshGen;

	meshGen.CreateBox(1.f, 1.f, 1, box);

	mBoxVertexOffset = 0;

	mBoxIndexCount   = box.Indices.size();

	mBoxIndexOffset  = 0;

	UINT totalVertexCount = box.Vertices.size();
	UINT totalIndexCount  = mBoxIndexCount;

	//apply the height function to each vertex
	//allocate  color for each vertex
	std::vector<Vertex::Basic32> vertices(totalVertexCount);
	for (int i = 0; i < box.Vertices.size(); i++)
	{
		vertices[i].Pos		 = box.Vertices[i].Position;
		vertices[i].Normal	 = box.Vertices[i].Normal;
		vertices[i].TangentU = box.Vertices[i].TangentU;
		vertices[i].Tex		 = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &pBoxVB));

	//create the index buffer

	//std::vector<UINT> indices;
	//indices.insert()
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &pBoxIB));
}

