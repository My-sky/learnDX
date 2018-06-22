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
	D3D11_BLEND_DESC alphaToCoverageBS = { 0 };
	alphaToCoverageBS.AlphaToCoverageEnable = true;
	alphaToCoverageBS.IndependentBlendEnable = false;
	alphaToCoverageBS.RenderTarget[0].BlendEnable = false;
	alphaToCoverageBS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&alphaToCoverageBS, &pAlphaToCoverageBS));

	//TransparentBS
	D3D11_BLEND_DESC transparentBS = { 0 };
	transparentBS.AlphaToCoverageEnable = false;
	transparentBS.IndependentBlendEnable = false;

	transparentBS.RenderTarget[0].BlendEnable = true;
	transparentBS.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	transparentBS.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC1_ALPHA;
	transparentBS.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	transparentBS.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	transparentBS.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;
	transparentBS.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	transparentBS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&transparentBS, &pTransparentBS));

}

void RenderStates::DestroyAll()
{
	ReleaseCOM(pWireframeRS);
	ReleaseCOM(pNoCullRS);
	ReleaseCOM(pAlphaToCoverageBS);
	ReleaseCOM(pTransparentBS);
}