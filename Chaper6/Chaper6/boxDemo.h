#ifndef BOXDEMO_H
#define BOXDEMO_H

#include "D3DApp.h"
#include "mathHelper.h"
#include "d3dx11effect.h"

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

class Box :public D3DApp
{
public:
	Box(HINSTANCE hInstance);
	~Box();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void CreateGeometryBuffers();//vertex index
	void CreateFX();
	void CreateLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;//mFX释放的时候会释放
	ID3DX11EffectMatrixVariable* mfxWorldViewProject;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

#endif