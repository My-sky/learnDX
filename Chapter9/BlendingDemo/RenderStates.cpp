#include"RenderStates.h"

ID3D11RasterizerState*	RenderStates::pWireframeRS = 0;
ID3D11RasterizerState*	RenderStates::pNoCullRS = 0;

ID3D11BlendState*		RenderStates::pAlphaToCoverageBS = 0;
ID3D11BlendState*		RenderStates::pTransparentBS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//WireframeRS
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &pWireframeRS));

	//NoCullRS
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &pNoCullRS));

	//AlphaToCoverageBS



}