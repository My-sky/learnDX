#ifndef WaveDEMO_H
#define WaveDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "Waves.h"
#include "LightHelper.h"

class Wave :public D3DApp
{
public:
	Wave(HINSTANCE hInstance);
	~Wave();

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

private:
	ID3D11Buffer* pHillVB;
	ID3D11Buffer* pHillIB;

	ID3D11Buffer* pWavesVB;
	ID3D11Buffer* pWavesIB;

	ID3D11ShaderResourceView* pGrassMapSRV;
	ID3D11ShaderResourceView* pWaveMapSRV;

	Waves mWaves;

	DirectionalLight mDirLights[3];
	Material mHillMat;
	Material mWavesMat;

	UINT mGridIndexCount;
	XMFLOAT2 mWaveTexOffset;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaveTexTransform;
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
