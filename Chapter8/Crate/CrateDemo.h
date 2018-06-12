#ifndef LIGHTINGDEMO_H
#define LIGHTINGDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "Waves.h"
#include "LightHelper.h"

struct CommonVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
};

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
	float GetHeight(float x, float z) const;
	XMFLOAT3 GetNormal(float x, float y) const;
	void CreateHillGeometryBuffers();//vertex index
	void CreateWaveGeometryBuffers();//vertex index
	void CreateFX();
	void CreateLayout();

private:
	ID3D11Buffer* pHillVB;
	ID3D11Buffer* pHillIB;

	ID3D11Buffer* pWavesVB;
	ID3D11Buffer* pWavesIB;

	Waves mWaves;
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	Material mHillMat;
	Material mWavesMat;

	ID3DX11Effect* pFX;
	ID3DX11EffectTechnique* pTech;//pFX释放的时候会释放
	ID3DX11EffectMatrixVariable* pfxWorldViewProject;
	ID3DX11EffectMatrixVariable* pfxWorld;
	ID3DX11EffectMatrixVariable* pfxWorldInvTranspose;
	ID3DX11EffectVectorVariable* pfxEyePosW;
	ID3DX11EffectVariable* pfxDirLight;
	ID3DX11EffectVariable* pfxPointLight;
	ID3DX11EffectVariable* pfxSpotLight;
	ID3DX11EffectVariable* pfxMaterial;

	ID3D11InputLayout* pInputLayout;

	UINT mGridIndexCount;

	XMFLOAT4X4 mHillWorld;
	XMFLOAT4X4 mWaveWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	XMFLOAT3 mEyePosW;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
