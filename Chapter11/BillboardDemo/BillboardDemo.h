#ifndef BLENDINGDEMO_H
#define BLENDINGDEMO_H

#include "D3DApp.h"
#include "d3dx11effect.h"
#include "mathHelper.h"
#include "MeshGenerator.h"
#include "Waves.h"
#include "LightHelper.h"

enum RenderOptions
{
	Lighting = 0,
	Textures = 1,
	TextureAndFog = 2
};
struct TreePointSprite
{
	XMFLOAT3 Pos;
	XMFLOAT2 Size;
};

class Billboard :public D3DApp
{
public:
	Billboard(HINSTANCE hInstance);
	~Billboard();

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
	void CreateBoxGeometryBuffers();
	void CreateTreePointLayout();

private:
	ID3D11Buffer* pHillVB;
	ID3D11Buffer* pHillIB;

	ID3D11Buffer* pWavesVB;
	ID3D11Buffer* pWavesIB;

	ID3D11Buffer* pBoxVB;
	ID3D11Buffer* pBoxIB;

	ID3D11InputLayout* pTreePointLayout;

	ID3D11Buffer* pTreeVB;

	ID3D11ShaderResourceView* pGrassMapSRV;
	ID3D11ShaderResourceView* pWaveMapSRV;
	ID3D11ShaderResourceView* pBoxMapSRV;

	Waves mWaves;

	DirectionalLight mDirLights[3];
	Material mHillMat;
	Material mWavesMat;
	Material mBoxMat;

	UINT mGridIndexCount;
	XMFLOAT2 mWaveTexOffset;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaveTexTransform;
	XMFLOAT4X4 mHillWorld;
	XMFLOAT4X4 mWaveWorld;
	XMFLOAT4X4 mBoxWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProject;

	XMFLOAT3 mEyePosW;

	RenderOptions mRenderOptions;

	float mThea;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
#endif
