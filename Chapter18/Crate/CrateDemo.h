#ifndef LIGHTINGDEMO_H
#define LIGHTINGDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"

class Crate :public D3DApp
{
public:
	Crate(HINSTANCE hInstance);
	~Crate();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void CreateGeometryBuffers();//vertex index

private:
	ID3D11Buffer* pBoxVB;
	ID3D11Buffer* pBoxIB;

	ID3D11ShaderResourceView* pDiffuseMapSRV;
	ID3D11ShaderResourceView* pNormalMapSRV;

	DirectionalLight mDirLights[3];
	Material mBoxMat;

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	int	 mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mEyePosW;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
