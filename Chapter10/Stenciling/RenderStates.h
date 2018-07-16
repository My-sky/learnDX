#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	//rasterizer states
	static ID3D11RasterizerState* pWireframeRS;
	static ID3D11RasterizerState* pNoCullRS;
	static ID3D11RasterizerState* pCullClockwiseRS;

	//blend states
	static ID3D11BlendState* pAlphaToCoverageBS;
	static ID3D11BlendState* pTransparentBS;
	static ID3D11BlendState* pNoRenderTargetWritesBS;

	//depth/stencil states
	static ID3D11DepthStencilState* pMarkMirrorDSS;
	static ID3D11DepthStencilState* pDrawReflectionDSS;
	static ID3D11DepthStencilState* pNoDoubleBlendDSS;
	
};
#endif // !RENDERSTATES_H

