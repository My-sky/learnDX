#include"RenderStates.h"

ID3D11RasterizerState*	RenderStates::pWireframeRS = 0;
ID3D11RasterizerState*	RenderStates::pNoCullRS = 0;
ID3D11RasterizerState*	RenderStates::pCullClockwiseRS = 0;

ID3D11BlendState*		RenderStates::pAlphaToCoverageBS = 0;
ID3D11BlendState*		RenderStates::pTransparentBS = 0;
ID3D11BlendState*		RenderStates::pNoRenderTargetWritesBS = 0;

ID3D11DepthStencilState* RenderStates::pMarkMirrorDSS = 0;
ID3D11DepthStencilState* RenderStates::pDrawReflectionDSS = 0;
ID3D11DepthStencilState* RenderStates::pNoDoubleBlendDSS = 0;

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

	//CullClockwiseRS
	D3D11_RASTERIZER_DESC CullClockwiseDesc;
	ZeroMemory(&CullClockwiseDesc, sizeof(D3D11_RASTERIZER_DESC));
	CullClockwiseDesc.FillMode = D3D11_FILL_SOLID;
	CullClockwiseDesc.CullMode = D3D11_CULL_BACK;
	CullClockwiseDesc.FrontCounterClockwise = true;
	CullClockwiseDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&CullClockwiseDesc, &pCullClockwiseRS));

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
	transparentBS.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
	transparentBS.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	transparentBS.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	transparentBS.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;
	transparentBS.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	transparentBS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&transparentBS, &pTransparentBS));

	//NoRenderTargetWrite
	D3D11_BLEND_DESC noRenderTargetWriteBS = { 0 };
	noRenderTargetWriteBS.AlphaToCoverageEnable = false;
	noRenderTargetWriteBS.IndependentBlendEnable = false;

	noRenderTargetWriteBS.RenderTarget[0].BlendEnable = false;
	noRenderTargetWriteBS.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetWriteBS.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetWriteBS.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWriteBS.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetWriteBS.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetWriteBS.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetWriteBS.RenderTarget[0].RenderTargetWriteMask =0 ;
	HR(device->CreateBlendState(&noRenderTargetWriteBS, &pNoRenderTargetWritesBS));

	//MarkMirrorDDS
	D3D11_DEPTH_STENCIL_DESC markMirrorDesc;
	markMirrorDesc.DepthEnable = true;
	markMirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	markMirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	markMirrorDesc.StencilEnable = true;
	markMirrorDesc.StencilReadMask = 0xff;
	markMirrorDesc.StencilWriteMask = 0xff;

	markMirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markMirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	markMirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markMirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	markMirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markMirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	markMirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markMirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	HR(device->CreateDepthStencilState(&markMirrorDesc, &pMarkMirrorDSS));

	//drawReflection
	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc;
	drawReflectionDesc.DepthEnable = true;
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawReflectionDesc.StencilEnable = true;
	drawReflectionDesc.StencilReadMask = 0xff;
	drawReflectionDesc.StencilWriteMask = 0xff;

	drawReflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	drawReflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&drawReflectionDesc, &pDrawReflectionDSS));

	//nodoubleblend
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;

	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;

	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;

	HR(device->CreateDepthStencilState(&noDoubleBlendDesc, &pNoDoubleBlendDSS));


}

void RenderStates::DestroyAll()
{
	ReleaseCOM(pWireframeRS);
	ReleaseCOM(pNoCullRS);
	ReleaseCOM(pCullClockwiseRS);

	ReleaseCOM(pAlphaToCoverageBS);
	ReleaseCOM(pTransparentBS);
	ReleaseCOM(pNoRenderTargetWritesBS);

	ReleaseCOM(pMarkMirrorDSS);
	ReleaseCOM(pDrawReflectionDSS);
	ReleaseCOM(pNoDoubleBlendDSS);
}