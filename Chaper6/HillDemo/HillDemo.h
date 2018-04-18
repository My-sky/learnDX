#ifndef HILLDEMO_H
#define HILLDEMO_H

#include"D3DApp.h"
#include"d3dx11effect.h"
#include"mathHelper.h"
#include "MeshGenerator.h"

struct CommonVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

class Hill :public D3DApp
{
public:
	Hill(HINSTANCE hInstance);
	~Hill();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHeight(float x, float z) const;
	void CreateGeometryBuffers();//vertex index
	void CreateFX();
	void CreateLayout();

private:
	ID3D11Buffer* pHillVB;
	ID3D11Buffer* pHillIB;

	ID3DX11Effect* pFX;
	ID3DX11EffectTechnique* pTech;//pFX释放的时候会释放
	ID3DX11EffectMatrixVariable* pfxWorldViewProject;

	ID3D11InputLayout* pInputLayout;

	UINT mGridIndexCount;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
