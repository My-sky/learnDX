#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* pWireframeRS;
	static ID3D11RasterizerState* pNoCullRS;

	static ID3D11BlendState* pAlphaToCoverageBS;
	static ID3D11BlendState* pTransparentBS;
	
};
#endif // !RENDERSTATES_H

