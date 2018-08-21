#ifndef BLENDINGDEMO_H
#define BLENDINGDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "Waves.h"
#include "LightHelper.h"

class Tessellation :public D3DApp
{
public:
	Tessellation(HINSTANCE hInstance);
	~Tessellation();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void CreateTessVB();
	void CreateTessLayout();

private:
	ID3D11Buffer* pTessVB;
	ID3D11InputLayout* pTessLayout;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	XMFLOAT3 mEyePosW;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
